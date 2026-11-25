#include <cerrno>
#include <cstdint>
#include <span>
#include <sys/types.h>
#include <unistd.h>

#include "io/unique_fd.hpp"
#include "log/logger.hpp"
#include "network/tcp_connection.hpp"

TcpConnection::TcpConnection(int fd, sockaddr_in address, IConnectionManager& manager)
    : ufd(UniqueFd(fd)), address(address), manager(manager) {}

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
    disconnect();
    return;
  }

  int consumed_bytes = client.handle_packets(std::span(read_buffer).subspan(read_cursor));
  read_cursor += consumed_bytes;
  logger::debug("Consumed {} bytes from read buffer, cursor position: {}", consumed_bytes, read_cursor);

  if (read_cursor > READ_CLEAR_THRESHOLD || read_cursor == int(read_buffer.size())) {
    read_buffer.erase(read_buffer.begin(), read_buffer.begin() + read_cursor);
    read_cursor = 0;
    logger::debug("Cleared read buffer");
  }

  send_outgoing();
}

void TcpConnection::on_write() {
  ssize_t bytes_written = write_bytes(std::span(write_buffer).subspan(write_cursor));

  if (bytes_written == -1) {
    if (errno == EAGAIN) {
      return;
    }
    disconnect();
    return;
  }

  write_cursor += bytes_written;

  if (write_cursor > WRITE_CLEAR_THRESHOLD || write_cursor == static_cast<int>(write_buffer.size())) {
    write_buffer.erase(write_buffer.begin(), write_buffer.begin() + write_cursor);
    write_cursor = 0;
    logger::debug("Cleared write buffer");
  }

  if (write_buffer.empty() || write_buffer.size() == size_t(write_cursor)) {
    write_buffer.clear();
    manager.disable_writing_flag(ufd.get_fd());
    if (client.disconnect_after_writing) {
      disconnect();
    }
    return;
  }
}

void TcpConnection::send_outgoing() {
  if (client.get_outgoing().empty()) {
    if (write_buffer.empty() && client.disconnect_after_writing) {
      disconnect();
    }
    return;
  }

  ssize_t sent = 0;

  if (write_buffer.empty()) {
    sent = write_bytes(client.get_outgoing());
  }

  if (sent == -1) {
    if (errno == EAGAIN) {
      sent = 0;
    } else {
      manager.disconnect(ufd.get_fd());
      return;
    }
  }

  if (sent < ssize_t(client.get_outgoing().size())) {
    write_buffer.insert(write_buffer.end(), client.get_outgoing().begin() + sent, client.get_outgoing().end());
    manager.enable_writing_flag(ufd.get_fd());
  }

  client.clear_outgoing();

  if (write_buffer.empty() && client.disconnect_after_writing) {
    disconnect();
  }
}

ssize_t TcpConnection::write_bytes(std::span<const uint8_t> buffer) { return write(ufd.get_fd(), buffer.data(), buffer.size()); }

void TcpConnection::disconnect() { manager.disconnect(ufd.get_fd()); }
