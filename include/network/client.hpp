#pragma once

#include <netinet/in.h>

#include "network/unique_fd.hpp"

class Client {
  public:
    UniqueFd fd;
    sockaddr_in address;

    Client(int fd, sockaddr_in address);
};
