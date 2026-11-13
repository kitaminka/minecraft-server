#pragma once

#include "io/epoll.hpp"
#include "io/unique_fd.hpp"

class SignalHandler : public IEpollHandler {
  UniqueFd ufd;
  Epoll& epoll;

  public:
    SignalHandler(Epoll& epoll);
    ~SignalHandler();
    void on_read();
};
