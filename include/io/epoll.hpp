#pragma once

#include <cstdint>
#include <functional>
#include <sys/epoll.h>

#include "io/epoll_handler.hpp"
#include "io/unique_fd.hpp"

class Epoll {
public:
  Epoll();

  void add(int fd, IEpollHandler *handler);
  void remove(int fd);

  void enable_writing_flag(int fd, IEpollHandler *handler);
  void disable_writing_flag(int fd, IEpollHandler *handler);

  void start_loop();
  void stop_loop();

  void set_post_process_callback(std::function<void()> callback);

private:
  static constexpr int MAX_EVENTS = 32;
  static constexpr int TIMEOUT = -1;

  UniqueFd epoll_ufd;

  epoll_event events[MAX_EVENTS];

  int wait();

  void modify(int fd, IEpollHandler *handler, int operation, uint32_t events);

  std::function<void()> post_process_callback;

  bool loop_running{false};
};
