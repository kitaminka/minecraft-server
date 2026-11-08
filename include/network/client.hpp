#pragma once

#include <netinet/in.h>

#include "io/unique_fd.hpp"

class Client {
  public:
    UniqueFd ufd;
    sockaddr_in address;

    Client(int fd, sockaddr_in address);
};
