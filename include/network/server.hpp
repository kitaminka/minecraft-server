#pragma once
#include <thread>
#include <atomic>
#include <netinet/in.h>
#include <unordered_map>

#include "network/client.hpp"
#include "network/unique_fd.hpp"

class Server {
  int port{0};

  UniqueFd socket_fd;
  UniqueFd epoll_fd;
  UniqueFd wakeup_fd;

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
