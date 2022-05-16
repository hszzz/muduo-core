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

  Channel(EventLoop* loop, int fd);
  ~Channel();

  void handleEvent(Timestamp receiveTime);

 private:
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
