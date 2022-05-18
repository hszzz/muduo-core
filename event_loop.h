#ifndef MUDUO_CORE_EVENT_LOOP_H
#define MUDUO_CORE_EVENT_LOOP_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "noncopyable.h"
#include "timestamp.h"
#include "current_thread.h"

namespace muduo_core {

class Channel;
class Poller;

class EventLoop : noncopyable {
 public:
  using Functor = std::function<void()>;

  EventLoop();
  ~EventLoop();

  void loop();
  void quit();

  Timestamp pollReturnTime() const { return pollRetuenTime_; }

  void runInLoop(Functor cb);
  void queueInLoop(Functor cb);

  void wakeup();
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  bool hasChannel(Channel* channel);

  bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

 private:
  void handleRead(); // wakeup
  void doPendingFunctors();

  using ChannelList = std::vector<Channel*>;

  std::atomic<bool> looping_;
  std::atomic<bool> quit_;
  // std::atomic<bool> eventHandling_;

  const pid_t threadId_;

  Timestamp pollRetuenTime_;
  std::unique_ptr<Poller> poller_;

  int wakeupFd_;
  std::unique_ptr<Channel> wakeupChannel_;  // eventfd

  ChannelList activeChannels_;
  // Channel* currentActiveChannel_;

  std::atomic<bool> callingPendingFunctors_;
  std::mutex mutex_;
  std::vector<Functor> pendingFunctors_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_EVENT_LOOP_H

