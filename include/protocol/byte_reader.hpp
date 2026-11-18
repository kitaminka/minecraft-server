#pragma once

#include <cstdint>
#include <vector>

class ByteReader {
  public:
    ByteReader(std::vector<uint8_t> buffer);
    int read_varint();
    uint8_t read_byte();
  private:
    std::vector<uint8_t> buffer;

    int cursor_pos{0};
};
