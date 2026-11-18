#pragma once

#include <cstdint>
#include <vector>

class MinecraftClient {
  public:
    void read_packet(std::vector<uint8_t>& buffer);
};
