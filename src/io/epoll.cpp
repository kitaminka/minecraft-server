#include <cerrno>
#include <ctime>
#include <sys/epoll.h>
#include <system_error>

#include "io/epoll.hpp"

Epoll::Epoll() : epoll_ufd(UniqueFd(epoll_create1(EPOLL_CLOEXEC))) {
  if (!epoll_ufd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating epoll failed");
  }
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
  return epoll_wait(epoll_ufd.get_fd(), events, max_events, timeout_ms);
}
