#include "protocol/minecraft_client.hpp"
#include "protocol/byte_reader.hpp"
#include "log/logger.hpp"

void MinecraftClient::read_frame(std::vector<uint8_t>& buffer) {
  ByteReader reader(buffer);
  logger::debug("{}", reader.read_varint());
}
