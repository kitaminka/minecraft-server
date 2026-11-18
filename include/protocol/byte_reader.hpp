#pragma once

#include <cstdint>
#include <vector>

class ByteReader {
  public:
    ByteReader(std::vector<uint8_t> buffer);
    int read_varint();
  private:
    std::vector<uint8_t> buffer;
};
