#include <iostream>
#include <memory>
#include <stdexcept>
#include <sys/socket.h>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <cstdint>

#include "network/server.hpp"
#include "network/client.hpp"
#include "io/unique_fd.hpp"

constexpr int MAX_EVENTS = 32;

Server::Server(int port) : port(port) {}

Server::~Server() {
  stop();
}

void Server::start() {
  std::cout << "Starting server on port " << port << std::endl;

  initialize_socket();
  initialize_epoll();

  is_running = true;

  network_thread = std::thread(&Server::epoll_loop, this);
}

void Server::stop() {
  is_running = false;

  write_wakeup();

  if(network_thread.joinable()) {
    network_thread.join();
  }

  clients.clear();
}

void Server::initialize_socket() {
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

void Server::initialize_epoll() {
  if (!socket_ufd.is_active()) {
    throw std::logic_error("Initalizing epoll before socket was created");
  }

  wakeup_ufd = UniqueFd(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC));
  if (!wakeup_ufd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating wakeup failed");
  }

  epoll.add(socket_ufd);
  epoll.add(wakeup_ufd);
}

void Server::epoll_loop() {
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
      if (event.data.fd == wakeup_ufd.get_fd()) {
        uint64_t value;
        read(wakeup_ufd.get_fd(), &value, sizeof(value));
        continue;
      } else if (event.data.fd == socket_ufd.get_fd()) {
        accept_clients();
      } else {
        // Client* client = static_cast<Client*>(event.data.ptr);
        Client* client = clients[event.data.fd].get();
        // TODO: add handling for cases when there is no client
        char buffer[100] = {0};
        int len = read(client->ufd.get_fd(), buffer, 100);
        if (len == 0) {
          int client_fd = client->ufd.get_fd();
          epoll.remove(client->ufd);
          clients.erase(client_fd);
          std::cout << "Removed client: " << client_fd << std::endl;
        } else {
          // TODO
        }
      }
    }
  }
}

void Server::write_wakeup() {
  uint64_t value = 1;
  write(wakeup_ufd.get_fd(), &value, sizeof(value));
}

void Server::request_stop() {
  is_running = false;
  this->write_wakeup();
}

void Server::accept_clients() {
  while(true) {
    sockaddr_in client_address;
    socklen_t client_addr_len = sizeof(client_address);
    int client_fd = accept4(socket_ufd.get_fd(), reinterpret_cast<sockaddr*>(&client_address), &client_addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (client_fd < 0) {
      std::cout << "No more clients to add" << std::endl;
      // TODO: add error handling
      break;
    } else {
      clients[client_fd] = std::make_unique<Client>(client_fd, client_address);

      // TODO: add error handling
      Client* client = clients[client_fd].get();
      epoll.add(client->ufd);

      std::cout << "Added client: " << client->ufd.get_fd() << std::endl;
    }
  }
}
