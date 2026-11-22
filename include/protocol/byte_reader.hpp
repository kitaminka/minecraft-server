#pragma once

#include <cstdint>
#include <span>
#include <string>

class ByteReader {
  public:
    ByteReader(std::span<const uint8_t> buffer);

    uint8_t read_byte();
    int read_varint();
    int64_t read_long();
    uint16_t read_unsigned_short();
    std::string read_string(int max_length);

    size_t remaining();
    int get_pos();
  private:
    std::span<const uint8_t> buffer;

    int cursor_pos{0};
};
