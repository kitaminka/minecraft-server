#pragma once

#include <netinet/in.h>
#include <vector>

#include "io/epoll.hpp"
#include "io/unique_fd.hpp"
#include "network/connection_manager.hpp"

class TcpConnection : public IEpollHandler {
  static constexpr int READ_CHUNK_SIZE = 4096;

  std::vector<uint8_t> read_buffer;

  public:
    UniqueFd ufd;
    sockaddr_in address;
    IConnectionManager &manager;

    TcpConnection(int fd, sockaddr_in address, IConnectionManager &manager);

    bool read_incoming();
    void on_read() override;
};
