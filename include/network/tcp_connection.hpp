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

    void on_read() override;
    void on_write() override;
  private:
    static constexpr int READ_CHUNK_SIZE = 4096;
    static constexpr int READ_CLEAR_THRESHOLD = 4096;

    static constexpr int WRITE_CLEAR_THRESHOLD = 4096;

    IConnectionManager &manager;
    MinecraftClient client;

    bool read_incoming();

    ssize_t write_bytes(std::span<const uint8_t> buffer);
    void send_outgoing();
    void disconnect();

    int read_cursor{0};
    int write_cursor{0};

    std::vector<uint8_t> read_buffer;
    std::vector<uint8_t> write_buffer;
};
