#pragma once

#include <netinet/in.h>
#include <vector>

#include "io/unique_fd.hpp"

class TcpConnection {
  static constexpr int READ_CHUNK_SIZE = 4096;

  std::vector<uint8_t> read_buffer;

  public:
    UniqueFd ufd;
    sockaddr_in address;

    TcpConnection(int fd, sockaddr_in address);
    bool read_incoming();
};
