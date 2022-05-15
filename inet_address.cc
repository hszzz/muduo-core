#include "inet_address.h"

#include <strings.h>

namespace muduo_core {

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
  ::bzero(&addr_, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = ::htons(port);
  addr_.sin_addr.s_addr = ::inet_addr(ip.c_str());
}

std::string InetAddress::toIp() const {
  char buf[64] = {0};
  ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  return buf;
}

std::string InetAddress::toIpPort() const {
  // TODO
  return toIp() + ":" +  std::to_string(toPort());
}

uint16_t InetAddress::toPort() const {
  return ::ntohs(addr_.sin_port);
}

}
