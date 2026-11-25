#pragma once
#include <memory>
#include <netinet/in.h>
#include <unordered_map>
#include <vector>

#include "io/epoll.hpp"
#include "io/unique_fd.hpp"
#include "network/tcp_connection.hpp"

class TcpServer : public IEpollHandler, public IConnectionManager {
public:
  explicit TcpServer(Epoll &epoll, int port);
  ~TcpServer();

  void start();
  void stop();

  void on_read() override;
  void disconnect(int fd) override;
  void enable_writing_flag(int fd) override;
  void disable_writing_flag(int fd) override;

private:
  static constexpr int LISTEN_QUEUE_SIZE = 128;

  int port{0};

  Epoll &epoll;

  UniqueFd socket_ufd;
  sockaddr_in server_address{};

  std::unordered_map<int, std::unique_ptr<TcpConnection>> connections;

  std::vector<int> pending_disconnect;

  void initialize_socket();
  void accept_connections();
  void process_disconnected();
};
