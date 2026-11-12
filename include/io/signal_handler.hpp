#pragma once

#include "io/epoll.hpp"
#include "io/unique_fd.hpp"

class SignalHandler : public IEpollHandler {
  UniqueFd ufd;
  Epoll& epoll;

  public:
    SignalHandler(Epoll& epoll);
    void on_read();
};
