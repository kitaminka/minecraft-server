#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>

#include "network/tcp_connection.hpp"
#include "io/unique_fd.hpp"

TcpConnection::TcpConnection(int fd, sockaddr_in address, IConnectionManager &manager) : ufd(UniqueFd(fd)), address(address), manager(manager) {}

// Returns whether client is disconnected or not
bool TcpConnection::read_incoming() {
  size_t old_size = read_buffer.size();
  read_buffer.resize(old_size + READ_CHUNK_SIZE);
  ssize_t bytes_read = read(ufd.get_fd(), read_buffer.data() + old_size, READ_CHUNK_SIZE);
  std::cout << "Read " << bytes_read << " bytes, current buffer: " << read_buffer.data() << std::endl;
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
    manager.remove_connection(ufd.get_fd());
  }
}
