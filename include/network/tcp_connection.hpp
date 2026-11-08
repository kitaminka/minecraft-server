#pragma once

#include <netinet/in.h>

#include "io/unique_fd.hpp"

class TcpConnection {
  public:
    UniqueFd ufd;
    sockaddr_in address;

    TcpConnection(int fd, sockaddr_in address);
};
