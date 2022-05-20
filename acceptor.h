#ifndef MUDUO_CORE_ACCEPTOR_H
#define MUDUO_CORE_ACCEPTOR_H

#include <functional>

#include "channel.h"
#include "noncopyable.h"
#include "socket.h"

namespace muduo_core {

class EventLoop;

class Acceptor : noncopyable {
 public:
  using NewConnectionCallback =
      std::function<void(int sockfd, const InetAddress&)>;

  Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnectionCallback& cb) {
    newConnectionCallback_ = cb;
  }

  bool listening() const { return listening_; }
  void listen();

 private:
  void handleRead();

  EventLoop* loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnectionCallback newConnectionCallback_;
  bool listening_;
};

}  // namespace muduo_core

#endif  //  MUDUO_CORE_ACCEPTOR_H
