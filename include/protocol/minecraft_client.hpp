#pragma once

#include <cstdint>
#include <span>

enum class State {
  Handshaking,
  Status,
  Login,
  Transfer,
  Configuration,
  Play
};

class MinecraftClient {
  public:
    int read_packet(std::span<const uint8_t> buffer);
  private:
    State state;
};
