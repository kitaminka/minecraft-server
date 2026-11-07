#pragma once
#include <thread>
#include <atomic>
#include <netinet/in.h>
#include <unordered_map>
#include "network/client.hpp"

class Server {
  int port{0};
  int socket_fd{-1};
  int epoll_fd{-1};
  int wakeup_fd{-1};

  std::atomic<bool> is_running{false};

  std::unordered_map<int, std::unique_ptr<Client>> clients;

  sockaddr_in server_address{};

  std::thread network_thread;

  void epoll_loop();
  void initialize_socket();
  void initialize_epoll();
  void write_wakeup();
  void accept_clients();

  public:
    Server(int port);
    ~Server();
    void start();
    void stop();
    void request_stop();
};
