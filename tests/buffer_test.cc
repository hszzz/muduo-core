#include <iostream>
#include <string>
#include "../buffer.h"

int main() {
  std::string str("111222333hello world");
  muduo_core::Buffer buffer;
  buffer.append(str.c_str(), str.length());
  std::cout << buffer.retrieveAsString(3) << std::endl;
  std::cout << buffer.retrieveAsString(3) << std::endl;
  std::cout << buffer.retrieveAsString(3) << std::endl;

  std::cout << buffer.retrieveAllAsString() << std::endl;
  return 0;
}

