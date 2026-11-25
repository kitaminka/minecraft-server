#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "log/logger.hpp"
#include "protocol/byte_reader.hpp"

ByteReader::ByteReader(std::span<const uint8_t> buffer) : buffer(buffer) {}

uint8_t ByteReader::read_byte() {
  if (size_t(cursor_pos) >= buffer.size()) {
    throw std::runtime_error("Trying to read outside buffer");
  }
  uint8_t value = buffer[cursor_pos];
  cursor_pos++;
  return value;
}

int ByteReader::read_varint() {
  int value = 0;

  for (int i = 0; i < 5; i++) {
    uint8_t byte = read_byte();

    value |= (byte & 0x7F) << (i * 7);

    if ((byte & 0x80) == 0) {
      logger::debug("Read VarInt: {}", value);
      return value;
    }
  }

  throw std::runtime_error("VarInt too long");
}

std::string ByteReader::read_string(int max_length) {
  int length = read_varint();

  if (length < 0 || size_t(length) > remaining() || length > max_length) {
    throw std::runtime_error("Invalid string length");
  }

  std::string res(buffer.begin() + cursor_pos, buffer.begin() + cursor_pos + length);

  cursor_pos += length;

  return res;
}

uint16_t ByteReader::read_unsigned_short() {
  uint8_t high = read_byte();
  uint8_t low = read_byte();
  return (static_cast<uint16_t>(high) << 8) | low;
}

int64_t ByteReader::read_long() {
  uint64_t res = 0;
  for (int i = 7; i >= 0; i--) {
    res |= static_cast<uint64_t>(read_byte()) << (i * 8);
  }
  return static_cast<int64_t>(res);
}

size_t ByteReader::remaining() { return buffer.size() - size_t(cursor_pos); }

int ByteReader::get_pos() { return cursor_pos; }

void ByteReader::skip(size_t count) { cursor_pos += count; }
