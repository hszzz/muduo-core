#ifndef MUDUO_CORE_EPOLL_POLLER_H
#define MUDUO_CORE_EPOLL_POLLER_H

#include <sys/epoll.h>
#include <unistd.h>

#include "poller.h"
#include "timestamp.h"

namespace muduo_core {

class EPollPoller : public Poller {
 public:
  explicit EPollPoller(EventLoop* loop);
  ~EPollPoller() override { ::close(epollfd_); }

  Timestamp poll(int timeoutMs, ChannelList* activateChannels) override;
  void updateChannel(Channel* channel) override;
  void removeChannel(Channel* channel) override;

 private:
  using EventList = std::vector<epoll_event>;

  static const int kInitEventListSize = 16;

  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
  void update(int operation, Channel* channel);

  int epollfd_;
  EventList events_;
};

}  // namespace muduo_core

#endif  // MUDUO_CORE_EPOLL_POLLER_H
