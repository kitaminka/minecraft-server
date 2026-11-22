#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <sys/epoll.h>

#include "network/tcp_server.hpp"
#include "network/tcp_connection.hpp"
#include "io/unique_fd.hpp"
#include "log/logger.hpp"

TcpServer::TcpServer(Epoll &epoll, int port) : port(port), epoll(epoll)  {}

TcpServer::~TcpServer() {
  stop();
}

void TcpServer::start() {
  logger::info("Starting server on port {}", port);

  initialize_socket();
  epoll.add(socket_ufd.get_fd(), this);
  epoll.set_post_process_callback([this](){ process_disconnected(); });
}

void TcpServer::stop() {
  connections.clear();
  epoll.set_post_process_callback(nullptr);
  epoll.remove(socket_ufd.get_fd());
  socket_ufd.reset();
}

void TcpServer::initialize_socket() {
  socket_ufd = UniqueFd(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0));
  if (!socket_ufd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating socket failed");
  }

  int opt = 1;
  if (setsockopt(socket_ufd.get_fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    throw std::system_error(errno, std::generic_category(), "setsockopt failed");
  }
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
  socklen_t server_addr_len = sizeof(server_address);
  if(bind(socket_ufd.get_fd(), reinterpret_cast<sockaddr*>(&server_address), server_addr_len) < 0) {
    throw std::system_error(errno, std::generic_category(), "bind failed");
  }
  if (listen(socket_ufd.get_fd(), LISTEN_QUEUE_SIZE) < 0) {
    throw std::system_error(errno, std::generic_category(), "listen failed");
  }
}

void TcpServer::accept_connections() {
  bool accepting = true;
  while(accepting) {
    sockaddr_in conn_addr;
    socklen_t conn_addr_len = sizeof(conn_addr);
    int conn_fd = accept4(socket_ufd.get_fd(), reinterpret_cast<sockaddr*>(&conn_addr), &conn_addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (conn_fd < 0) {
      accepting = false;
    } else {
      connections[conn_fd] = std::make_unique<TcpConnection>(conn_fd, conn_addr, *this);

      // TODO: add error handling
      TcpConnection* conn = connections[conn_fd].get();
      epoll.add(conn->ufd.get_fd(), conn);

      logger::info("Added connection: {}", conn->ufd.get_fd());
    }
  }
}

void TcpServer::disconnected(int fd) {
  pending_disconnect.push_back(fd);
}

void TcpServer::enable_writing_flag(int fd) {
  epoll.enable_writing_flag(fd, connections[fd].get());
}

void TcpServer::disable_writing_flag(int fd) {
  epoll.disable_writing_flag(fd, connections[fd].get());
}

void TcpServer::on_read() {
  accept_connections();
}

void TcpServer::process_disconnected() {
  for (int fd : pending_disconnect) {
    epoll.remove(fd);
    connections.erase(fd);
    logger::info("Removed connection: {}", fd);
  }
  pending_disconnect.clear();
}
