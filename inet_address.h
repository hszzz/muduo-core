#ifndef MUDUO_CORE_INET_ADDR_H
#define MUDUO_CORE_INET_ADDR_H

#include <arpa/inet.h>

#include <string>

namespace muduo_core {

class InetAddress {
 public:
  InetAddress() : InetAddress("127.0.0.1", 0) {}
  explicit InetAddress(const std::string& ip) : InetAddress(ip, 0) {}
  explicit InetAddress(uint16_t port) : InetAddress("127.0.0.1", port) {}

  explicit InetAddress(const std::string& ip, uint16_t port);

  explicit InetAddress(const sockaddr_in& addr) : addr_(addr) {}

  std::string toIp() const;
  std::string toIpPort() const;
  uint16_t toPort() const;

  const sockaddr_in* getSockAddr() const { return &addr_; }
  void setSockAddr(const sockaddr_in& addr) { addr_ = addr; }

 private:
  sockaddr_in addr_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_INET_ADDR_H
