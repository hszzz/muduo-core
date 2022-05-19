#include "../thread.h"

#include <iostream>

int main() {
  muduo_core::Thread thread([]() { 
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    std::cout << "thread 1" << std::endl;
  });
  thread.start();
  // thread.join();
  std::cout << thread.name() << std::endl;
  std::cout << thread.tid() << std::endl;
  std::cout << "thread end" << std::endl;
  return 0;
}
