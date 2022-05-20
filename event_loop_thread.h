#ifndef MUDUO_CORE_EVENT_LOOP_THREAD_H
#define MUDUO_CORE_EVENT_LOOP_THREAD_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>

#include "noncopyable.h"
#include "thread.h"

namespace muduo_core {

class EventLoop;

class EventLoopThread : noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop*)>;

  explicit EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                           const std::string& name = std::string());
  ~EventLoopThread();

  EventLoop* startLoop();

 private:
  void threadFunc();

  EventLoop* loop_;
  bool exiting_;
  Thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_;
  ThreadInitCallback callback_;
};

}  // namespace muduo_core

#endif  //  MUDUO_CORE_EVENT_LOOP_THREAD_H
