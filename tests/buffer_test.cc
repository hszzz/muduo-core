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

  muduo_core::Buffer buffer1;
  buffer1.append("11111111", 8);
  std::cout << buffer1.peek() << std::endl;
  buffer1.retrieve(8);
  buffer1.append("2", 1);
  std::cout << buffer1.peek() << std::endl;
  std::cout << buffer1.retrieveAllAsString() << std::endl;

  return 0;
}

