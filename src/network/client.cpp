#include <unistd.h>

#include "network/client.hpp"

Client::Client(int fd, sockaddr_in address) {
  this->fd = fd;
  this->address = address;
}

Client::~Client() {
  close(fd);
}
