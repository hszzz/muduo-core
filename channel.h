#ifndef MUDUO_CORE_CHANNEL_H
#define MUDUO_CORE_CHANNEL_H

#include <functional>
#include <memory>

#include "noncopyable.h"
#include "timestamp.h"

namespace muduo_core {

class EventLoop;

class Channel : noncopyable {
 public:
  using EventCallBack = std::function<void()>;
  using ReadEventCallback = std::function<void(Timestamp)>;

  Channel(EventLoop* loop, int fd)
      : loop_(loop), fd_(fd), events_(0), index_(-1), tied_(false) {}
  ~Channel() = default;

  void handleEvent(Timestamp receiveTime);

  void setReadCallBack(ReadEventCallback cb) { readCallBack_ = std::move(cb); }
  void setWriteCallBack(EventCallBack cb) { writeCallBack_ = std::move(cb); }
  void setCloseCallBack(EventCallBack cb) { closeCallBack_ = std::move(cb); }
  void setErrorCallBack(EventCallBack cb) { errorCallBack_ = std::move(cb); }

  // 防止 channel 在执行任务中途被手动 remove 掉
  void tie(const std::shared_ptr<void>&);

  int fd() const { return fd_; }
  int events() const { return events_; }
  void set_revents(int revts) { revents_ = revts; }

  void enableReading() {
    events_ |= kReadEvent;
    update();
  }
  void disableReading() {
    events_ &= ~kReadEvent;
    update();
  }
  void enableWriting() {
    events_ |= kWriteEvent;
    update();
  }
  void disableWriting() {
    events_ &= ~kWriteEvent;
    update();
  }
  void disableAll() {
    events_ = kNoneEvent;
    update();
  }

  // 返回fd当前的事件状态
  bool isNoneEvent() const { return events_ == kNoneEvent; }
  bool isWriting() const { return events_ & kWriteEvent; }
  bool isReading() const { return events_ & kReadEvent; }

  int index() { return index_; }
  void set_index(int idx) { index_ = idx; }

  // one loop per thread
  EventLoop* ownerLoop() { return loop_; }
  void remove();

 private:
  void update();
  void handleEventWithGuard(Timestamp receiveTime);

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop* loop_;
  const int fd_;
  int events_;
  int revents_;
  int index_;

  std::weak_ptr<void> tie_;
  bool tied_;

  ReadEventCallback readCallBack_;
  EventCallBack writeCallBack_;
  EventCallBack closeCallBack_;
  EventCallBack errorCallBack_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_CHANNEL_H
