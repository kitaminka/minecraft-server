#pragma once

#include <cstdint>
#include <span>

class MinecraftClient {
  public:
    int read_packet(std::span<const uint8_t> buffer);
};
