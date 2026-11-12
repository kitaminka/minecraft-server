#pragma once

#include <sys/epoll.h>

#include "io/unique_fd.hpp"

class IEpollHandler{
  public:
    virtual void on_read() = 0;
    virtual ~IEpollHandler() = default;
};

class Epoll {
  static constexpr int MAX_EVENTS = 32;
  static constexpr int TIMEOUT = -1;

  UniqueFd epoll_ufd;

  epoll_event events[MAX_EVENTS];

  int wait();

  bool loop_running{false};

  public:
    Epoll();

    void add(int fd, IEpollHandler *handler);
    void remove(int fd);
    void start_loop();
    void stop_loop();
};
