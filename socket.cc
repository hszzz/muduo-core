#include "socket.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <netinet/tcp.h>

#include "inet_address.h"
#include "logger.h"

namespace muduo_core {

Socket::~Socket() { ::close(sockfd_); }

void Socket::bindAddress(const InetAddress& localAddr) {
  if (0 != ::bind(sockfd_,
             reinterpret_cast<const sockaddr*>(localAddr.getSockAddr()),
             sizeof(sockaddr_in))) {
    LOG_FATAL("bind sockfd: %d fail", sockfd_);
  }
}

void Socket::listen() {
  if (0 != ::listen(sockfd_, 1024)) {
    LOG_FATAL("listen sockfd: %d fail", sockfd_);
  }
}

int Socket::accept(InetAddress* peerAddress) {
  sockaddr_in addr;
  socklen_t len = sizeof(addr);
  ::bzero(&addr, sizeof(addr));
  int connfd = ::accept(sockfd_, (sockaddr*)&addr, &len);
  if (connfd >= 0) {
    peerAddress->setSockAddr(addr);
  }
  return connfd;
}

void Socket::shutdownWrite() {
  if (::shutdown(sockfd_, SHUT_WR) < 0) {
    LOG_ERROR("shutdownWrite error");
  }
}

void Socket::setTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

}  // namespace muduo_core
