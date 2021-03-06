#include "../logger.h"

#include <atomic>
#include <thread>

std::atomic<int> counter(0);

void producer() {
  while (1) {
    LOG_INFO("messgae: %s no%d", "test", counter++);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

int main() {
  LOG_INFO("%s %s", "123", "123");
  LOG_DEBUG("123");
  // LOG_FATAL("123");
  LOG_ERROR("123");

  std::thread t1(producer);
  std::thread t2(producer);
  t1.join();
  t2.join();
}
