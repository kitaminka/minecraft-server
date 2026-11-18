#include <cstdint>
#include <stdexcept>
#include <vector>
#include <system_error>

#include "protocol/byte_reader.hpp"

ByteReader::ByteReader(std::vector<uint8_t> buffer) : buffer(buffer) {}

int ByteReader::read_varint() {
  int value = 0;

  for (int i = 0; i < 5; i++) {
    if (static_cast<size_t>(i) >= buffer.size()) {
      throw std::runtime_error("Cannot read full VarInt");
    }
    uint8_t byte = buffer[i];

    value |= (byte & 0x7F) << (i * 7);
    if ((byte & 0x80) == 0) {
      return value;
    }
  }

  throw std::runtime_error("VarInt too long");
}
