#ifndef MUDUO_CORE_SOCKET_H
#define MUDUO_CORE_SOCKET_H

#include "noncopyable.h"

namespace muduo_core {

class InetAddress;

class Socket : noncopyable {
 public:
  explicit Socket(int sockfd) : sockfd_(sockfd) {}
  ~Socket();

  int fd() const { return sockfd_; }
  void bindAddress(const InetAddress& localAddr);
  void listen();
  int accept(InetAddress* peerAddress);

  void shutdownWrite();

  void setTcpNoDelay(bool on);
  void setReuseAddr(bool on);
  void setReusePort(bool on);
  void setKeepAlive(bool on);

 private:
   const int sockfd_;
};

}  // namespace muduo_core

#endif  //  MUDUO_CORE_SOCKET_H
