#pragma once

#include <functional>
#include <sys/epoll.h>

#include "io/unique_fd.hpp"
#include "io/epoll_handler.hpp"

class Epoll {
  public:
    Epoll();

    void add(int fd, IEpollHandler *handler);
    void remove(int fd);
    void start_loop();
    void stop_loop();

    void set_post_process_callback(std::function<void()> callback);
  private:
    static constexpr int MAX_EVENTS = 32;
    static constexpr int TIMEOUT = -1;

    UniqueFd epoll_ufd;

    epoll_event events[MAX_EVENTS];

    int wait();

    std::function<void()> post_process_callback;

    bool loop_running{false};
};
