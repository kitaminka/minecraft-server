#include "protocol/byte_writer.hpp"
#include <bit>
#include <cstdint>

ByteWriter::ByteWriter(std::vector<uint8_t>& buffer) : buffer(buffer) {}

void ByteWriter::write_varint(int value) {
  while ((value & ~0x7F) != 0) {
    write_byte((value & 0x7F) | 0x80);
    value = static_cast<uint32_t>(value) >> 7;
  }
  write_byte(value);
}

void ByteWriter::write_byte(uint8_t value) {
  buffer.push_back(value);
}

void ByteWriter::write_string(std::string_view value) {
  write_varint(value.size());
  buffer.insert(buffer.end(), value.begin(), value.end());
}

void ByteWriter::write_long(int64_t value) {
  auto uvalue = static_cast<uint64_t>(value);
  for (int i = 56; i >= 0; i -= 8) {
    write_byte((uvalue >> i) & 0xFF);
  }
}

int ByteWriter::get_varint_size(int value) {
  if (value == 0) return 1;
  int bits = 32 - std::countl_zero(static_cast<uint32_t>(value));
  return (bits + 6) / 7;
}
