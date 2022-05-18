#include <stdlib.h>

#include "epoll_poller.h"
#include "poller.h"

namespace muduo_core {

Poller* Poller::newDefaultPoller(EventLoop* loop) {
  if (::getenv("MUDUO_USE_POLL")) {
    // TODO
    // return nullptr;
  }
  return new EPollPoller(loop);
}

}  // namespace muduo_core
