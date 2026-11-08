#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "network/tcp_server.hpp"
#include "network/tcp_connection.hpp"
#include "io/unique_fd.hpp"

constexpr int MAX_EVENTS = 32;

TcpServer::TcpServer(int port) : port(port) {}

TcpServer::~TcpServer() {
  stop();
}

void TcpServer::start() {
  std::cout << "Starting server on port " << port << std::endl;

  initialize_socket();
  epoll.add(socket_ufd);

  is_running = true;

  network_thread = std::thread(&TcpServer::epoll_loop, this);
}

void TcpServer::stop() {
  is_running = false;

  epoll.wakeup();

  if(network_thread.joinable()) {
    network_thread.join();
  }

  connections.clear();
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
  socklen_t server_addrlen = sizeof(server_address);
  if(bind(socket_ufd.get_fd(), reinterpret_cast<sockaddr*>(&server_address), server_addrlen) < 0) {
    throw std::system_error(errno, std::generic_category(), "bind failed");
  }
  if (listen(socket_ufd.get_fd(), 3) < 0) {
    throw std::system_error(errno, std::generic_category(), "listen failed");
  }
}

void TcpServer::epoll_loop() {
  epoll_event events[MAX_EVENTS];
  int n;

  while(is_running) {
    n = epoll.wait(events, MAX_EVENTS);

    if (n < 0) {
      break;
    }

    std::cout << "Received " << n << " event(s)" << std::endl;

    for (int i = 0; i < n; i++) {
      epoll_event event = events[i];
      if (event.data.fd == socket_ufd.get_fd()) {
        accept_connections();
      } else {
        // TcpConnection* tcp_connection = static_cast<TcpConnection*>(event.data.ptr);
        // TODO: replace with find method
        TcpConnection* connection = connections[event.data.fd].get();
        // TODO: add handling for cases when there is no tcp_connection
        char buffer[100] = {0};
        int len = read(connection->ufd.get_fd(), buffer, 100);
        if (len == 0) {
          int connection_fd = connection->ufd.get_fd();
          epoll.remove(connection->ufd);
          connections.erase(connection_fd);
          std::cout << "Removed tcp_connection: " << connection_fd << std::endl;
        } else {
          // TODO
        }
      }
    }
  }
}

void TcpServer::request_stop() {
  is_running = false;
  epoll.wakeup();
}

void TcpServer::accept_connections() {
  while(true) {
    sockaddr_in tcp_connection_address;
    socklen_t tcp_connection_addr_len = sizeof(tcp_connection_address);
    int tcp_connection_fd = accept4(socket_ufd.get_fd(), reinterpret_cast<sockaddr*>(&tcp_connection_address), &tcp_connection_addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (tcp_connection_fd < 0) {
      std::cout << "No more tcp_connections to add" << std::endl;
      // TODO: add error handling
      break;
    } else {
      connections[tcp_connection_fd] = std::make_unique<TcpConnection>(tcp_connection_fd, tcp_connection_address);

      // TODO: add error handling
      TcpConnection* tcp_connection = connections[tcp_connection_fd].get();
      epoll.add(tcp_connection->ufd);

      std::cout << "Added tcp_connection: " << tcp_connection->ufd.get_fd() << std::endl;
    }
  }
}
