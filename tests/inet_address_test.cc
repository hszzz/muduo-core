#include "../inet_address.h"

#include <iostream>

int main() {
  muduo_core::InetAddress addr;
  std::cout << addr.toIp() << std::endl;
  std::cout << addr.toIpPort() << std::endl;
  std::cout << addr.toPort() << std::endl;

  muduo_core::InetAddress addr1("1.1.1.1");
  std::cout << addr1.toIp() << std::endl;
  std::cout << addr1.toIpPort() << std::endl;
  std::cout << addr1.toPort() << std::endl;

  muduo_core::InetAddress addr2(8888);
  std::cout << addr2.toIp() << std::endl;
  std::cout << addr2.toIpPort() << std::endl;
  std::cout << addr2.toPort() << std::endl;

  muduo_core::InetAddress addr3("8.8.8.8", 8888);
  std::cout << addr3.toIp() << std::endl;
  std::cout << addr3.toIpPort() << std::endl;
  std::cout << addr3.toPort() << std::endl;
}
