#pragma once
#include <thread>
#include <atomic>
#include <netinet/in.h>

class Server {
  int port{0};
  int server_fd{-1};

  std::atomic<bool> is_running{false};

  sockaddr_in server_address{};
  socklen_t server_addrlen{sizeof(server_address)};

  std::thread network_thread;

  void networkLoop();
  public:
    Server(int port);
    ~Server();
    void start();
    void stop();
};
