#include <iostream>
#include "network/server.hpp"

Server::Server(int port) {
  this->port = port;
}

void Server::start() {
  std::cout << "Starting server on port "  << port << std::endl;
}
