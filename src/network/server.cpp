#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>
#include "network/server.hpp"

Server::Server(int port) {
  this->port = port;
}

Server::~Server() {
  stop();
}

void Server::start() {
  int opt = 1;

  std::cout << "Starting server on port " << port << std::endl;
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    throw std::system_error(errno, std::generic_category(), "creating socket failed");
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    throw std::system_error(errno, std::generic_category(), "setsockopt failed");
  }
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
  if(bind(server_fd, (sockaddr*)&server_address, server_addrlen) < 0) {
    throw std::system_error(errno, std::generic_category(), "bind failed");
  }
  if (listen(server_fd, 3) < 0) {
    throw std::system_error(errno, std::generic_category(), "listen failed");
  }

  is_running = true;

  network_thread = std::thread(&Server::networkLoop, this);
}

void Server::stop() {
  std::cout << "Stopping server..." << std::endl;

  is_running = false;

  if (server_fd > 0) {
    shutdown(server_fd, SHUT_RDWR);
    close(server_fd);
    server_fd = -1;
  }

  if(network_thread.joinable()) {
    network_thread.join();
  }
}

void Server::networkLoop() {
  int new_socket;
  constexpr int BUFFER_SIZE = 1024;

  while(is_running) {
    char buffer[BUFFER_SIZE] = {0};
    sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);

    new_socket = accept(server_fd, (sockaddr*)&client_address, &client_addrlen);
    std::cout << "Client connected: " << inet_ntoa(client_address.sin_addr) << std::endl;
    ssize_t valread = read(new_socket, buffer, BUFFER_SIZE);
    std::cout << "Received: " << buffer << std::endl;
    send(new_socket, buffer, valread, 0);
    std::cout << "Echo message sent" << std::endl;
    close(new_socket);
  }
}
