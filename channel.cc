#include "channel.h"

#include <sys/epoll.h>

#include "event_loop.h"
#include "logger.h"

namespace muduo_core {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

void Channel::tie(const std::shared_ptr<void>& obj) {
  tie_ = obj;
  tied_ = true;
}

void Channel::update() { loop_->updateChannel(this); }

void Channel::remove() { loop_->removeChannel(this); }

void Channel::handleEvent(Timestamp receiveTime) {
  if (tied_) {
    std::shared_ptr<void> guard = tie_.lock();
    if (guard) {
      handleEventWithGuard(receiveTime);
    }
  } else {
    handleEventWithGuard(receiveTime);
  }
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
  LOG_INFO("channel handle events: %d", revents_);

  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
    if (closeCallBack_) closeCallBack_();
  }

  if (revents_ & EPOLLERR) {
    if (errorCallBack_) errorCallBack_();
  }

  if (revents_ & (EPOLLIN | EPOLLPRI)) {
    if (readCallBack_) readCallBack_(receiveTime);
  }

  if (revents_ & EPOLLOUT) {
    if (writeCallBack_) writeCallBack_();
  }
}

}  // namespace muduo_core
