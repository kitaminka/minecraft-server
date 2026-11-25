#pragma once

#include "protocol/byte_reader.hpp"
#include "protocol/byte_writer.hpp"

#include <string>

struct LoginStart {
  std::string username;

  static LoginStart read(ByteReader &reader) { return {.username = reader.read_string(16)}; }
};

struct LoginSuccess {
  static constexpr int ID = 0x02;

  std::string uuid;
  std::string username;

  void write(ByteWriter &writer) const {
    writer.write_string(uuid);
    writer.write_string(username);
  }
};
