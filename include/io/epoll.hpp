#pragma once

#include <sys/epoll.h>

#include "io/unique_fd.hpp"

class Epoll {
  UniqueFd epoll_ufd;

  public:
    Epoll();

    void add(const UniqueFd& ufd);
    void remove(const UniqueFd& ufd);
    int wait(epoll_event* events, int max_events, int timeout_ms = -1);
};
