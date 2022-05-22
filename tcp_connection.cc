#include "tcp_connection.h"

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <functional>

#include "channel.h"
#include "event_loop.h"
#include "logger.h"
#include "socket.h"

namespace muduo_core {

static EventLoop* checkLoopNotNull(EventLoop* loop) {
  if (loop == nullptr) {
    LOG_FATAL("mainloop is nullptr!");
  }
  return loop;
}

TcpConnection::TcpConnection(EventLoop* loop, const std::string& nameArg,
                             int sockfd, const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    : loop_(checkLoopNotNull(loop)),
      name_(nameArg),
      state_(kConnecting),
      reading_(true),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop, sockfd)),
      localAddr_(localAddr),
      peerAddr_(peerAddr),
      highWaterMark_(64 * 1024 * 1024) {
  channel_->setReadCallBack(
      std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
  channel_->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallBack(std::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallBack(std::bind(&TcpConnection::handleError, this));

  LOG_INFO("TcpConnection::ctor[%s] at fd=%d", name_.c_str(), sockfd);
  socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
  LOG_INFO("TcpConnection::dtor[%s] at fd=%d state=%d", name_.c_str(),
           channel_->fd(), (int)state_);
}

void TcpConnection::handleRead(Timestamp receiveTime) {
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
  if (n > 0) {
    messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
  } else if (n == 0) {
    handleClose();
  } else {
    errno = savedErrno;
    LOG_ERROR("TcpConnection::handleRead");
    handleError();
  }
}

void TcpConnection::handleWrite() {
  if (channel_->isWriting()) {
    int savedErrno = 0;
    ssize_t n = outputBuffer_.writeFd(channel_->fd(), &savedErrno);
    if (n > 0) {
      outputBuffer_.retrieve(n);
      if (outputBuffer_.readableBytes() == 0) {
        channel_->disableWriting();
        if (writeCompleteCallback_) {
          loop_->queueInLoop(
              std::bind(writeCompleteCallback_, shared_from_this()));
        }

        if (state_ == kDisconnecting) {
          shutdownInLoop();
        }
      }
    } else {
      LOG_ERROR("TcpConnection::handleWrite");
    }
  } else {
    LOG_ERROR("TcpConnection fd = %d is down, no more writing", channel_->fd());
  }
}

void TcpConnection::handleClose() {
  LOG_INFO("fd=%d state=%d", channel_->fd(), (int)state_);
  setState(kDisconnected);
  channel_->disableAll();

  TcpConnectionPtr guardThis(shared_from_this());
  connectionCallback_(guardThis);
  closeCallback_(guardThis);
}

void TcpConnection::handleError() {
  int optval;
  socklen_t optlen = sizeof(optval);
  int err = 0;
  if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen)) {
    err = errno;
  } else {
    err = optval;
  }
  LOG_ERROR("TcpConnection::handleError name:%s - SO_ERROR:%d", name_.c_str(),
            err);
}

void TcpConnection::send(const void* message, int len) {
  if (state_ == kConnected) {
    if (loop_->isInLoopThread()) {
      sendInLoop(message, len);
    } else {
      loop_->runInLoop(
          std::bind(&TcpConnection::sendInLoop, this, message, len));
    }
  }
}

void TcpConnection::shutdown() {
  if (state_ == kConnected) {
    setState(kDisconnecting);
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
  }
}

void TcpConnection::connectEstablished() {
  setState(kConnected);
  channel_->tie(shared_from_this());
  channel_->enableReading();

  connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
  if (state_ == kConnected) {
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
  }
  channel_->remove();
}

void TcpConnection::sendInLoop(const void* message, size_t len) {
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;

  if (state_ == kDisconnected) {
    LOG_ERROR("disconnected... give up writing");
    return;
  }

  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
    remaining = len - nwrote;
    if (remaining == 0 && writeCompleteCallback_) {
      loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
    }
  } else {
    nwrote = 0;
    if (errno != EWOULDBLOCK) {
      LOG_ERROR("TcpConnection::sendInLoop");
      if (errno == EPIPE || errno == ECONNRESET) {
        faultError = true;
      }
    }
  }

  if (!faultError && remaining > 0) {
    size_t oldlen = outputBuffer_.readableBytes();
    if (oldlen + remaining >= highWaterMark_ && oldlen < highWaterMark_ &&
        highWaterMarkCallback_) {
      loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(),
                                   oldlen + remaining));
    }
    outputBuffer_.append((char*)message + nwrote, remaining);
    if (!channel_->isWriting()) {
      channel_->enableWriting();
    }
  }
}

void TcpConnection::shutdownInLoop() {
  if (!channel_->isWriting()) {
    socket_->shutdownWrite();
  }
}

}  // namespace muduo_core
