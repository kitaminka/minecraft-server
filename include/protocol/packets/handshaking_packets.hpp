#pragma once

#include "protocol/byte_reader.hpp"
#include <cstdint>
#include <string>

enum class Intent {
  Status = 1,
  Login = 2
};

struct Intention {
  static constexpr int MAX_SERVER_ADDRESS_LENGTH = 200;

  int protocol_version;
  std::string server_address;
  uint16_t server_port;
  Intent intent;

  static Intention read(ByteReader& reader) {
    return {
      .protocol_version = reader.read_varint(),
      .server_address = reader.read_string(MAX_SERVER_ADDRESS_LENGTH),
      .server_port = reader.read_unsigned_short(),
      .intent = static_cast<Intent>(reader.read_varint())
    };
  }
};
