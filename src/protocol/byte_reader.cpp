#include <cstdint>
#include <stdexcept>
#include <vector>

#include "protocol/byte_reader.hpp"
#include "log/logger.hpp"

ByteReader::ByteReader(std::vector<uint8_t> buffer) : buffer(buffer) {}

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

uint8_t ByteReader::read_byte() {
  if (static_cast<size_t>(cursor_pos) >= buffer.size()) {
    throw std::runtime_error("Trying to read outside buffer");
  }
  uint8_t value = buffer[cursor_pos];
  cursor_pos++;
  return value;
}
