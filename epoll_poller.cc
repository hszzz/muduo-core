#include "epoll_poller.h"

#include <strings.h>

#include "channel.h"
#include "logger.h"

namespace muduo_core {

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop* loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize) {
  if (epollfd_ < 0) {
    LOG_FATAL("epoll create error: %d", errno);
  }
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activateChannels) {
  LOG_INFO("function: %s, channel size: %ld", __FUNCTION__,
           activateChannels->size());

  int numEvents = ::epoll_wait(epollfd_, &*events_.begin(),
                               static_cast<int>(events_.size()), timeoutMs);
  int saveErrno = errno;
  Timestamp now(Timestamp::now());

  if (numEvents > 0) {
    LOG_INFO("%d events happened", numEvents);
    fillActiveChannels(numEvents, activateChannels);
    if (static_cast<size_t>(numEvents) == events_.size()) events_.resize(events_.size() * 2);
  } else if (numEvents == 0) {
    LOG_DEBUG("epoll timeout");
  } else {
    if (saveErrno != EINTR) {
      errno = saveErrno;
      LOG_ERROR("epoll error: %d", errno);
    }
  }

  return now;
}

void EPollPoller::updateChannel(Channel* channel) {
  const int index = channel->index();
  LOG_INFO("function: %s, fd: %d events: %d, index: %d", __FUNCTION__,
           channel->fd(), channel->events(), index);

  if (index == kNew || index == kDeleted) {
    if (index == kNew) {
      int fd = channel->fd();
      channels_[fd] = channel;
    }

    channel->set_index(kAdded);
    update(EPOLL_CTL_ADD, channel);
  } else {
    // int fd = channel->fd();
    if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL, channel);
      channel->set_index(kDeleted);
    } else {
      update(EPOLL_CTL_MOD, channel);
    }
  } 
}

void EPollPoller::removeChannel(Channel* channel) {
  int fd = channel->fd();
  channels_.erase(fd);

  LOG_INFO("remove channel");

  int index = channel->index();
  if (index == kAdded) {
    update(EPOLL_CTL_DEL, channel);
  }
  channel->set_index(kNew);
}

void EPollPoller::fillActiveChannels(int numEvents,
                                     ChannelList* activeChannels) const {
  for (int i = 0; i < numEvents; ++i) {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->set_revents(events_[i].events);
    activeChannels->push_back(channel);
  }
}

void EPollPoller::update(int operation, Channel* channel) {
  epoll_event event;
  ::bzero(&event, sizeof(event));

  int fd = channel->fd();

  event.events = channel->events();
  event.data.fd = fd;
  event.data.ptr = channel;

  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    if (operation == EPOLL_CTL_DEL) {
      LOG_ERROR("epoll ctl del error");
    } else {
      LOG_ERROR("epoll ctl add/mod error");
    }
  }
}

}  // namespace muduo_core
