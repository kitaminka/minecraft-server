#pragma once

#include <cstdint>
#include <vector>

class MinecraftClient {
  public:
    void read_frame(std::vector<uint8_t>& buffer);
};
