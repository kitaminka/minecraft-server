#pragma once

#include <cstdint>

#include "protocol/byte_reader.hpp"
#include "protocol/byte_writer.hpp"

struct StatusRequest {
  // Does not have any fields
};

struct PingRequest {
  int64_t timestamp;

  static PingRequest read(ByteReader &reader) { return {.timestamp = reader.read_long()}; }
};

struct StatusResponse {
  static constexpr int ID = 0x00;

  std::string json_status;

  void write(ByteWriter &writer) const { writer.write_string(json_status); }
};

struct PingResponse {
  static constexpr int ID = 0x01;

  int64_t timestamp;

  void write(ByteWriter &writer) const { writer.write_long(timestamp); }
};
