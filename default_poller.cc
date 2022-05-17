#include "poller.h"
#include "stdlib.h"

namespace muduo_core {

Poller* Poller::newDefaultPoller(EventLoop* loop) {
  if (::getenv("MUDUO_USE_POLL"))
  {
    return nullptr;
  }
  return nullptr;
}

}

