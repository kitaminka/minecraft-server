#pragma once

#include <cstdint>
#include <span>

class ByteReader {
  public:
    ByteReader(std::span<const uint8_t> buffer);
    int read_varint();
    uint8_t read_byte();
    size_t remaining();
    int get_pos();
  private:
    std::span<const uint8_t> buffer;

    int cursor_pos{0};
};
