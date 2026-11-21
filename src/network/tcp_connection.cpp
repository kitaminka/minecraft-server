#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

#include "network/tcp_connection.hpp"
#include "io/unique_fd.hpp"
#include "log/logger.hpp"

TcpConnection::TcpConnection(int fd, sockaddr_in address, IConnectionManager &manager) : ufd(UniqueFd(fd)), address(address), manager(manager) {}

// Returns whether client is disconnected or not
bool TcpConnection::read_incoming() {
  size_t old_size = read_buffer.size();
  read_buffer.resize(old_size + READ_CHUNK_SIZE);
  ssize_t bytes_read = read(ufd.get_fd(), read_buffer.data() + old_size, READ_CHUNK_SIZE);
  logger::debug("Read {} bytes", bytes_read);
  if (bytes_read <= 0) {
    read_buffer.resize(old_size);
  }
  if (bytes_read == 0) {
    return true;
  }
  if (bytes_read == -1) {
    if (errno == EAGAIN) {
      return false;
    }
    return true;
  }

  read_buffer.resize(old_size + bytes_read);

  return false;
}

void TcpConnection::on_read() {
  if (read_incoming()) {
    manager.disconnected(ufd.get_fd());
  } else {
    int consumed_bytes = client.read_packet(read_buffer);
    read_buffer.erase(read_buffer.begin(), read_buffer.begin() + consumed_bytes);
  }
}
