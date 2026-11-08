#include <cerrno>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <system_error>
#include <unistd.h>

#include "io/epoll.hpp"

Epoll::Epoll() : epoll_ufd(UniqueFd(epoll_create1(EPOLL_CLOEXEC))) {
  if (!epoll_ufd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating epoll failed");
  }

  wakeup_ufd = UniqueFd(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC));
  if (!wakeup_ufd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating wakeup failed");
  }

  add(wakeup_ufd);
}

void Epoll::add(const UniqueFd &ufd) {
  epoll_event socket_event;
  socket_event.events = EPOLLIN;
  socket_event.data.fd = ufd.get_fd();

  if(epoll_ctl(epoll_ufd.get_fd(), EPOLL_CTL_ADD, ufd.get_fd(), &socket_event) < 0) {
    throw std::system_error(errno, std::generic_category(), "Adding server socket to epoll failed");
  }
}

void Epoll::remove(const UniqueFd &ufd) {
  epoll_ctl(epoll_ufd.get_fd(), EPOLL_CTL_DEL, ufd.get_fd(), nullptr);
}

int Epoll::wait(epoll_event *events, int max_events, int timeout_ms) {
  int event_count = epoll_wait(epoll_ufd.get_fd(), events, max_events, timeout_ms);
  if (event_count <= 0) {
    return event_count;
  }

  int real_event_count = 0;
  for (int i = 0; i < event_count; i++) {
    epoll_event event = events[i];
    if (event.data.fd == wakeup_ufd.get_fd()) {
      uint64_t value;
      read(wakeup_ufd.get_fd(), &value, sizeof(value));
    } else {
      events[real_event_count++] = events[i];
    }
  }

  return  real_event_count;
}

void Epoll::wakeup() {
  uint64_t value = 1;
  write(wakeup_ufd.get_fd(), &value, sizeof(value));
}
