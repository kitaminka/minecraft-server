#include <cstdint>
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
    return;
  }

  int consumed_bytes = 1;

  while (consumed_bytes != 0 && read_cursor < static_cast<int>(read_buffer.size())) {
    consumed_bytes = client.handle_packet(std::span<const uint8_t>(read_buffer).subspan(read_cursor));
    read_cursor += consumed_bytes;
    logger::debug("Consumed {} bytes from read buffer, cursor position: {}", consumed_bytes, read_cursor);
  }

  logger::debug("Ended buffer reading iteration");
  if (read_cursor > READ_CLEAR_THRESHOLD || read_cursor == static_cast<int>(read_buffer.size())) {
    read_buffer.erase(read_buffer.begin(), read_buffer.begin() + read_cursor);
    read_cursor = 0;
    logger::debug("Cleared read buffer");
  }

  if (!client.get_outgoing().empty()) {
    write_buffer.insert(write_buffer.end(), client.get_outgoing().begin(), client.get_outgoing().end());
    client.clear_outgoing();
    manager.enable_writing_flag(ufd.get_fd());
  }
}

void TcpConnection::on_write() {
  int bytes_written = write(ufd.get_fd(), write_buffer.data() + write_cursor, write_buffer.size() - write_cursor);

  if (bytes_written == -1) {
    if (errno == EAGAIN) {
      return;
    }
    manager.disconnected(ufd.get_fd());
    return;
  }

  write_cursor += bytes_written;

  if (write_cursor > WRITE_CLEAR_THRESHOLD || write_cursor == static_cast<int>(write_buffer.size())) {
    write_buffer.erase(write_buffer.begin(), write_buffer.begin() + write_cursor);
    write_cursor = 0;
    logger::debug("Cleared write buffer");
  }

  if (write_buffer.empty() || write_buffer.size() == static_cast<size_t>(write_cursor)) {
    write_buffer.clear();
    manager.disable_writing_flag(ufd.get_fd());
    if (client.disconnect_after_writing) {
      manager.disconnected(ufd.get_fd());
    }
    return;
  }
}
