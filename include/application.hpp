#pragma once

#include "io/epoll.hpp"
#include "io/signal_handler.hpp"
#include "network/tcp_server.hpp"

class Application {
  Epoll epoll;
  TcpServer server;
  SignalHandler signal_handler;

  public:
    Application();
    void start();
    void stop();
};
