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
#include "network/unique_fd.hpp"

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
  socket_fd = UniqueFd(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0));
  if (!socket_fd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating socket failed");
  }

  int opt = 1;
  if (setsockopt(socket_fd.get_fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    throw std::system_error(errno, std::generic_category(), "setsockopt failed");
  }
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
  socklen_t server_addrlen = sizeof(server_address);
  if(bind(socket_fd.get_fd(), reinterpret_cast<sockaddr*>(&server_address), server_addrlen) < 0) {
    throw std::system_error(errno, std::generic_category(), "bind failed");
  }
  if (listen(socket_fd.get_fd(), 3) < 0) {
    throw std::system_error(errno, std::generic_category(), "listen failed");
  }
}

void Server::initialize_epoll() {
  if (!socket_fd.is_active()) {
    throw std::logic_error("Initalizing epoll before socket was created");
  }

  epoll_fd = UniqueFd(epoll_create1(EPOLL_CLOEXEC));
  if (!epoll_fd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating epoll failed");
  }

  wakeup_fd = UniqueFd(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC));
  if (!wakeup_fd.is_active()) {
    throw std::system_error(errno, std::generic_category(), "Creating wakeup failed");
  }

  epoll_event socket_event;
  socket_event.events = EPOLLIN;
  socket_event.data.fd = socket_fd.get_fd();
  if(epoll_ctl(epoll_fd.get_fd(), EPOLL_CTL_ADD, socket_fd.get_fd(), &socket_event) < 0) {
    throw std::system_error(errno, std::generic_category(), "Adding server socket to epoll failed");
  }

  epoll_event wakeup_event;
  wakeup_event.events = EPOLLIN;
  wakeup_event.data.fd = wakeup_fd.get_fd();
  if(epoll_ctl(epoll_fd.get_fd(), EPOLL_CTL_ADD, wakeup_fd.get_fd(), &wakeup_event) < 0) {
    throw std::system_error(errno, std::generic_category(), "Adding wakeup to epoll failed");
  }
}

void Server::epoll_loop() {
  epoll_event events[MAX_EVENTS];
  int n;

  while(is_running) {
    n = epoll_wait(epoll_fd.get_fd(), events, MAX_EVENTS, -1);

    if (n < 0) {
      break;
    }

    std::cout << "Received " << n << " event(s)" << std::endl;

    for (int i = 0; i < n; i++) {
      epoll_event event = events[i];
      if (event.data.fd == wakeup_fd.get_fd()) {
        uint64_t value;
        read(wakeup_fd.get_fd(), &value, sizeof(value));
        continue;
      } else if (event.data.fd == socket_fd.get_fd()) {
        accept_clients();
      } else {
        Client* client = static_cast<Client*>(event.data.ptr);
        // TODO: add handling for cases when there is no client
        char buffer[100] = {0};
        int len = read(client->fd.get_fd(), buffer, 100);
        if (len == 0) {
          int client_fd = client->fd.get_fd();
          epoll_ctl(epoll_fd.get_fd(), EPOLL_CTL_DEL, client_fd, nullptr);
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
  write(wakeup_fd.get_fd(), &value, sizeof(value));
}

void Server::request_stop() {
  is_running = false;
  this->write_wakeup();
}

void Server::accept_clients() {
  while(true) {
    sockaddr_in client_address;
    socklen_t client_addr_len = sizeof(client_address);
    int client_fd = accept4(socket_fd.get_fd(), reinterpret_cast<sockaddr*>(&client_address), &client_addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (client_fd < 0) {
      std::cout << "No more clients to add" << std::endl;
      // TODO: add error handling
      break;
    } else {
      auto client_unique = std::make_unique<Client>(client_fd, client_address);

      epoll_event client_event;
      client_event.events = EPOLLIN;
      client_event.data.ptr = client_unique.get();

      clients[client_fd] = std::move(client_unique);

      // TODO: add error handling
      epoll_ctl(epoll_fd.get_fd(), EPOLL_CTL_ADD, client_fd, &client_event);

      Client* client = clients[client_fd].get();
      std::cout << "Added client: " << client->fd.get_fd() << std::endl;
    }
  }
}
