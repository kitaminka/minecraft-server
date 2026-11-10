#pragma once
#include <cstdint>
#include <thread>
#include <atomic>
#include <netinet/in.h>
#include <unordered_map>
#include <memory>
#include <vector>

#include "network/tcp_connection.hpp"
#include "io/unique_fd.hpp"
#include "io/epoll.hpp"

class TcpServer {
  int port{0};

  Epoll epoll;

  UniqueFd socket_ufd;

  std::atomic<bool> is_running{false};

  std::unordered_map<int, std::unique_ptr<TcpConnection>> connections;

  sockaddr_in server_address{};

  std::thread network_thread;

  void epoll_loop();
  void initialize_socket();
  void accept_connections();

  public:
    explicit TcpServer(int port);
    ~TcpServer();
    void start();
    void stop();
};
