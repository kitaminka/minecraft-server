#pragma once

#include "protocol/byte_reader.hpp"
#include <cstdint>
#include <span>
#include <vector>

enum class State {
  Handshaking,
  Status,
  Login,
  Transfer,
  Configuration,
  Play
};

enum class Intent {
  Status = 1,
  Login = 2,
  Transfer = 3
};

class MinecraftClient {
  public:
    int handle_packet(std::span<const uint8_t> buffer);
    std::span<const uint8_t> get_outgoing();
    void clear_outgoing();
    bool disconnect_after_writing{false};
  private:
    State state{State::Handshaking};

    std::vector<uint8_t> outgoing;

    void handle_packet_data(int packet_id, ByteReader& reader);

    void handle_intention(ByteReader& reader);
    void handle_status_request();
    void handle_ping_request(ByteReader& reader);
};
