#include <exception>

#include "protocol/minecraft_client.hpp"
#include "protocol/byte_reader.hpp"
#include "log/logger.hpp"

void MinecraftClient::read_packet(std::vector<uint8_t>& buffer) {
  ByteReader reader(buffer);
  try {
    int packet_length = reader.read_varint();
  } catch (const std::exception& e) {
    logger::error("Failed to read packet lenght: {}", e.what());
  }
}
