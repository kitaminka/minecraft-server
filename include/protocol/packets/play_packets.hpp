#pragma once

#include "protocol/byte_writer.hpp"

#include <cstdint>
#include <string>

enum class Dimension {
  Nether = -1,
  Overworld = 0,
  End = 1
};

enum class Difficulty {
  Peaceful,
  Easy,
  Normal,
  Hard
};

struct JoinGame {
  static constexpr int ID = 0x23;

  int32_t entity_id{0};
  uint8_t gamemode{0};
  Dimension dimension{Dimension::Overworld};
  Difficulty difficulty{Difficulty::Peaceful};
  uint8_t max_players{0};
  std::string level_type{"default"};
  bool reduced_debug_info{false};

  void write(ByteWriter &writer) const {
    writer.write_int(entity_id);
    writer.write_byte(gamemode);
    writer.write_int(static_cast<int32_t>(dimension));
    writer.write_byte(static_cast<uint8_t>(difficulty));
    writer.write_byte(max_players);
    writer.write_string(level_type);
    writer.write_bool(reduced_debug_info);
  }
};
