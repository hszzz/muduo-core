#ifndef MUDUO_CORE_THREAD_H
#define MUDUO_CORE_THREAD_H

#include <unistd.h>

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <atomic>

#include "noncopyable.h"

namespace muduo_core {

class Thread : noncopyable {
 public:
  using ThreadFunc = std::function<void()>;

  explicit Thread(ThreadFunc func, const std::string& name = std::string());
  ~Thread();

  void start();
  void join();

  bool started() const { return started_; }
  pid_t tid() const { return tid_; }
  const std::string& name() const { return name_; }

  static int numCreated() { return numCreated_; }

 private:
  void setDefaultName();

  bool started_;
  bool joined_;
  std::shared_ptr<std::thread> thread_;
  pid_t tid_;
  ThreadFunc func_;
  std::string name_;
  static std::atomic<int> numCreated_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_THREAD_H
