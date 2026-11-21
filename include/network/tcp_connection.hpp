#pragma once

#include <netinet/in.h>
#include <vector>

#include "io/epoll.hpp"
#include "io/unique_fd.hpp"
#include "network/connection_manager.hpp"
#include "protocol/minecraft_client.hpp"

class TcpConnection : public IEpollHandler {
  public:
    UniqueFd ufd;
    sockaddr_in address;

    TcpConnection(int fd, sockaddr_in address, IConnectionManager &manager);

    bool read_incoming();
    void on_read() override;
  private:
    static constexpr int READ_CHUNK_SIZE = 4096;

    IConnectionManager &manager;
    MinecraftClient client;

    std::vector<uint8_t> read_buffer;
    std::vector<uint8_t> write_buffer;
};
