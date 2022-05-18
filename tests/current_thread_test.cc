#include "../current_thread.h"

#include <iostream>
#include <thread>

void func() {
  std::cout << muduo_core::CurrentThread::tid() << std::endl;
}

int main() {
  std::thread t1(func);
  std::thread t2(func);

  t1.join();
  t2.join();

  func();
}

