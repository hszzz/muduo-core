#include "event_loop.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include <memory>

#include "channel.h"
#include "logger.h"
#include "poller.h"

namespace muduo_core {

// 防止一个线程创建多个 EventLoop
__thread EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 1000;

int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG_FATAL("event fd error: %d", errno);
  }
  return evtfd;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this)),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      callingPendingFunctors_(false) {
  LOG_DEBUG("EventLoop created: %p in thread: %d", this, threadId_);
  if (t_loopInThisThread) {
    LOG_FATAL("Another EventLoop %p exists in this thread %d \n",
              t_loopInThisThread, threadId_);
  } else {
    t_loopInThisThread = this;
  }

  wakeupChannel_->setReadCallBack(std::bind(&EventLoop::handleRead, this));
  wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
  wakeupChannel_->disableAll();
  wakeupChannel_->remove();
  ::close(wakeupFd_);
  t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
  looping_ = true;
  quit_ = false;

  LOG_INFO("EventLoop %p start looping", this);

  while (!quit_) {
    activeChannels_.clear();
    pollRetuenTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
    // eventHandling_

    for (auto channel : activeChannels_) {
      channel->handleEvent(pollRetuenTime_);
    }

    // 执行当前 EventLoop(subloop) 事件循环中需要处理的回调操作
    // mainLoop 注册的回调
    doPendingFunctors();
  }

  LOG_INFO("EventLoop %p stop looping", this);
  looping_ = false;
}

void EventLoop::quit() {
  quit_ = true;
  if (!isInLoopThread()) {
    wakeup();
  }
}

void EventLoop::runInLoop(Functor cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(cb);
  }
}

void EventLoop::queueInLoop(Functor cb) {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    pendingFunctors_.emplace_back(cb);
  }

  if (!isInLoopThread() || callingPendingFunctors_) {
    wakeup();
  }
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n", n);
  }
}

void EventLoop::updateChannel(Channel* channel) {
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
  poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) {
  return poller_->hasChannel(channel);
}

void EventLoop::handleRead() {
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8", n);
  }
}

void EventLoop::doPendingFunctors() {
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;

  {
    std::unique_lock<std::mutex> lock(mutex_);
    functors.swap(pendingFunctors_);
  }

  for (const auto& functor : functors) {
    functor();
  }

  callingPendingFunctors_ = false;
}

}  // namespace muduo_core
