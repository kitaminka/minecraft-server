#include <cerrno>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <system_error>
#include <unistd.h>

#include "io/epoll.hpp"
#include "log/logger.hpp"

Epoll::Epoll() : epoll_ufd(UniqueFd(epoll_create1(EPOLL_CLOEXEC))) {
  if (!epoll_ufd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating epoll failed");
  }
}

void Epoll::add(int fd, IEpollHandler *handler) {
  modify(fd, handler, EPOLL_CTL_ADD, EPOLLIN);
}

void Epoll::enable_writing_flag(int fd, IEpollHandler *handler) {
  modify(fd, handler, EPOLL_CTL_MOD, EPOLLIN | EPOLLOUT);
}

void Epoll::disable_writing_flag(int fd, IEpollHandler *handler) {
  modify(fd, handler, EPOLL_CTL_MOD, EPOLLIN);
}

void Epoll::remove(int fd) {
  modify(fd, nullptr, EPOLL_CTL_DEL, 0);
}

void Epoll::modify(int fd, IEpollHandler *handler, int op, uint32_t events) {
  std::string op_s;
  switch (op) {
    case EPOLL_CTL_ADD:
      op_s = "Adding";
      break;
    case EPOLL_CTL_DEL:
      op_s = "Deleting";
      break;
    case EPOLL_CTL_MOD:
      op_s = "Modifying";
      break;
  }

  epoll_event socket_event;
  if (handler) {
    socket_event.events = events;
    socket_event.data.ptr = handler;
  }

  if(epoll_ctl(epoll_ufd.get_fd(), op, fd, &socket_event) < 0) {
    throw std::system_error(errno, std::generic_category(), std::format("Epoll: {} socket failed", op_s));
  }

  logger::debug("Epoll: {} socket {} succeeded", op_s, fd);
}

int Epoll::wait() {
  int event_count = epoll_wait(epoll_ufd.get_fd(), events, MAX_EVENTS, TIMEOUT);
  return event_count;
}

void Epoll::stop_loop() {
  loop_running = false;
}

void Epoll::start_loop() {
  loop_running = true;

  while(loop_running) {
    int event_count = wait();

    logger::debug("Received {} event(s)", event_count);

    if (event_count < 0 && errno != EINTR) {
      loop_running = false;
    } else {
      for (int i = 0; i < event_count; i++) {
        epoll_event event = events[i];

        auto handler = static_cast<IEpollHandler*>(event.data.ptr);

        if (event.events & EPOLLIN) {
          handler->on_read();
        }
        if (event.events & EPOLLOUT) {
          handler->on_write();
        }
      }
    }

    if (post_process_callback) {
      post_process_callback();
    }
  }
}

void Epoll::set_post_process_callback(std::function<void()> callback) {
  post_process_callback = callback;
}
