#pragma once

#include "io/epoll.hpp"
#include "io/signal_handler.hpp"
#include "network/tcp_server.hpp"

class Application {
  public:
    Application();

    void start();
    void stop();
  private:
    Epoll epoll;
    SignalHandler signal_handler;
    TcpServer server;
};
