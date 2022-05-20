#ifndef MUDUO_CORE_EVENT_LOOP_THREAD_POOL_H
#define MUDUO_CORE_EVENT_LOOP_THREAD_POOL_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "noncopyable.h"

namespace muduo_core {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable {
 public:
  using ThreadInitCallBack = std::function<void(EventLoop*)>;

  EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg);
  ~EventLoopThreadPool() = default;

  void setThreadNum(int numThreads) { numThreads_ = numThreads; }

  void start(const ThreadInitCallBack& cb = ThreadInitCallBack());

  // 轮询分配 channel
  EventLoop* getNextLoop();
  std::vector<EventLoop*> getAllLoops();

  bool started() const { return started_; }
  const std::string& name() const { return name_; }

 private:
  EventLoop* baseLoop_;
  std::string name_;
  bool started_;
  int numThreads_;
  int next_;

  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop*> loops_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_EVENT_LOOP_THREAD_POOL_H
