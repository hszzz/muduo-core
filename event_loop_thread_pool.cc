#include "event_loop_thread_pool.h"
#include "event_loop_thread.h"

#include <memory>
namespace muduo_core {
EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop,
                                         const std::string& nameArg)
    : baseLoop_(baseLoop),
      name_(nameArg),
      started_(false),
      numThreads_(0),
      next_(0) {}

void EventLoopThreadPool::start(const ThreadInitCallBack& cb) {
  started_ = true;

  for (int i=0; i<numThreads_; ++i) {
    char buf[name_.size() + 32];
    ::snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
    EventLoopThread* t = new EventLoopThread(cb, buf);
    threads_.push_back(std::unique_ptr<EventLoopThread>(t));
    loops_.push_back(t->startLoop());
  }

  // 整个服务端只有一个 mainLoop
  if (numThreads_ == 0 && cb) {
    cb(baseLoop_);
  }
}

// 轮询分配 channel
EventLoop* EventLoopThreadPool::getNextLoop() {
  EventLoop* loop = baseLoop_;

  if (!loops_.empty()) {
    loop = loops_[next_];
    ++next_;

    if (static_cast<size_t>(next_) >= loops_.size()) next_ = 0;
  }

  return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
  if (loops_.empty()) {
    return std::vector<EventLoop*>(1, baseLoop_);
  } else {
    return loops_;
  }
}

}  // namespace muduo_core
