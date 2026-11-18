#include <exception>

#include "protocol/minecraft_client.hpp"
#include "protocol/byte_reader.hpp"
#include "log/logger.hpp"

int MinecraftClient::read_packet(std::span<const uint8_t> buffer) {
  ByteReader reader(buffer);
  try {
    size_t packet_length = reader.read_varint();
    if (reader.remaining() < packet_length) {
      return 0;
    }
    int read = reader.get_pos() + packet_length;
    int packet_id = reader.read_varint();
    logger::debug("Got packet with ID: {}", packet_id);
    return read;
  } catch (const std::exception& e) {
    logger::debug("Failed to read packet length: {}", e.what());
    return 0;
  }
}
