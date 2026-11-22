#pragma once

#include <cstdint>
#include <vector>
#include <string_view>

class ByteWriter {
  public:
    static int get_varint_size(int value);

    ByteWriter(std::vector<uint8_t>& buffer);

    void write_varint(int value);
    void write_byte(uint8_t value);
    void write_string(std::string_view value);
    void write_long(int64_t value);
  private:
    std::vector<uint8_t>& buffer;
};
