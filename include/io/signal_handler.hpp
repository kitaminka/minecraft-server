#pragma once

#include "io/epoll.hpp"
#include "io/unique_fd.hpp"

class SignalHandler : public IEpollHandler {
  public:
    SignalHandler(Epoll& epoll);
    ~SignalHandler();

    void on_read() override;
  private:
    UniqueFd ufd;
    Epoll& epoll;
};
