#pragma once
#include <memory>
#include <netinet/in.h>
#include <unordered_map>
#include <memory>

#include "network/tcp_connection.hpp"
#include "io/unique_fd.hpp"
#include "io/epoll.hpp"

class TcpServer : public IEpollHandler, public IConnectionManager {
  int port{0};

  Epoll &epoll;

  UniqueFd socket_ufd;

  std::unordered_map<int, std::unique_ptr<TcpConnection>> connections;

  sockaddr_in server_address{};

  void initialize_socket();
  void accept_connections();

  public:
    explicit TcpServer(Epoll& epoll, int port);
    ~TcpServer();
    void start();
    void stop();

    void on_read() override;
    void remove_connection(int fd) override;
};
