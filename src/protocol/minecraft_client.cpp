#include <cstdint>
#include <exception>

#include "protocol/minecraft_client.hpp"
#include "protocol/byte_reader.hpp"
#include "log/logger.hpp"
#include "protocol/byte_writer.hpp"

int MinecraftClient::handle_packet(std::span<const uint8_t> buffer) {
  ByteReader reader(buffer);
  try {
    size_t packet_length = reader.read_varint();
    if (reader.remaining() < packet_length) {
      return 0;
    }
    int read = reader.get_pos() + packet_length;
    int packet_id = reader.read_varint();
    logger::debug("Got packet with ID: {}", packet_id);

    handle_packet_data(packet_id, reader);

    return read;
  } catch (const std::exception& e) {
    logger::debug("Failed to read packet length: {}", e.what());
    return 0;
  }
}

void MinecraftClient::handle_packet_data(int packet_id, ByteReader& reader) {
  switch (state) {
    case State::Handshaking:
      if (packet_id == 0) handle_intention(reader);
      break;
    case State::Status:
      if (packet_id == 0) handle_status_request();
      if (packet_id == 1) handle_ping_request(reader);
      break;
  }
}

void MinecraftClient::handle_intention(ByteReader& reader) {
  int protocol_version = reader.read_varint();
  std::string server_address = reader.read_string(200);
  uint16_t server_port = reader.read_unsigned_short();
  Intent intent = static_cast<Intent>(reader.read_varint());
  logger::debug("Protocol version: {} Server address: {} Port: {}", protocol_version, server_address, server_port);

  switch (intent) {
    case Intent::Status:
      state = State::Status;
      logger::debug("Setting state to Status");
      break;
    case Intent::Login:
      state = State::Login;
      logger::debug("Setting state to Login");
      break;
    case Intent::Transfer:
      state = State::Transfer;
      logger::debug("Setting state to Transfer");
      break;
  }
}

void MinecraftClient::handle_status_request() {
  ByteWriter writer(outgoing);

  const std::string status = "{ \"version\": { \"name\": \"1.21.8\", \"protocol\": 777 }, \"players\": { \"max\": 20, \"online\": 1, \"sample\": [ { \"name\": \"Player\", \"id\": \"4566e69f-c907-48ee-8d71-d7ba5aa00d20\" } ] }, \"description\": { \"text\": \"Hello, world!\" }, \"favicon\": \"data:image/png;base64,<data>\", \"enforcesSecureChat\": false }";

  writer.write_varint(status.size() + ByteWriter::get_varint_size(0) + ByteWriter::get_varint_size(status.size()));
  writer.write_varint(0);
  writer.write_string(status);
}

void MinecraftClient::handle_ping_request(ByteReader &reader) {
  int64_t timestamp = reader.read_long();

  ByteWriter writer(outgoing);

  writer.write_varint(ByteWriter::get_varint_size(1) + sizeof(uint64_t));
  writer.write_varint(1);
  writer.write_long(timestamp);
  disconnect_after_writing = true;
}

std::span<const uint8_t> MinecraftClient::get_outgoing() {
  return outgoing;
}

void MinecraftClient::clear_outgoing() {
  outgoing.clear();
}
