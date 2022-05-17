#include "poller.h"

#include "channel.h"

namespace muduo_core {

bool Poller::hasChannel(Channel* channel) const {
  auto it = channels_.find(channel->fd());
  return it != channels_.end() && it->second == channel;
}

}  // namespace muduo_core
