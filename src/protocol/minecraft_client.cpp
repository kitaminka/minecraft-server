#include <cstdint>
#include <exception>
#include <span>

#include "log/logger.hpp"
#include "protocol/byte_reader.hpp"
#include "protocol/minecraft_client.hpp"
#include "protocol/packets/handshaking_packets.hpp"
#include "protocol/packets/status_packets.hpp"

int MinecraftClient::handle_packets(std::span<const uint8_t> buffer) {
  ByteReader reader(buffer);
  int bytes_consumed = 0;

  while (reader.remaining() > 0) {
    size_t packet_length = 0;
    try {
      packet_length = reader.read_varint();
    } catch (const std::exception &e) {
      logger::debug("Failed to read packet length: {}", e.what());
      break;
    }

    if (reader.remaining() < packet_length) {
      logger::debug("Not full packet received");
      break;
    }

    int id_start = reader.get_pos();
    int packet_id = 0;
    try {
      packet_id = reader.read_varint();
    } catch (const std::exception &e) {
      logger::error("Failed to read packet ID: {}", e.what());
      disconnect_after_writing = true;
      break;
    }
    logger::debug("Got packet with ID: {}", packet_id);
    size_t id_length = reader.get_pos() - id_start;

    if (id_length > packet_length) {
      logger::error("Received packet length smaller than packet ID");
      disconnect_after_writing = true;
      break;
    }

    size_t payload_size = packet_length - id_length;
    auto packet_data = buffer.subspan(reader.get_pos(), payload_size);
    ByteReader data_reader(packet_data);

    handle_packet_data(packet_id, data_reader);

    reader.skip(payload_size);
    bytes_consumed = reader.get_pos();
  }

  return bytes_consumed;
}

void MinecraftClient::handle_packet_data(int packet_id, ByteReader &reader) {
  switch (state) {
  case State::Handshaking:
    handshaking_handler(packet_id, reader);
    break;
  case State::Status:
    status_handler(packet_id, reader);
    break;
  case State::Login:
    break;
  case State::Play:
    break;
  }
}

void MinecraftClient::handshaking_handler(int packet_id, ByteReader &reader) {
  switch (packet_id) {
  case 0x00:
    handle_intention(reader);
    break;
  }
}

void MinecraftClient::status_handler(int packet_id, ByteReader &reader) {
  switch (packet_id) {
  case 0x00:
    handle_status_request();
    break;
  case 0x01:
    handle_ping_request(reader);
    break;
  }
}

void MinecraftClient::handle_intention(ByteReader &reader) {
  Intention packet = Intention::read(reader);
  logger::debug("Protocol version: {} Server address: {} Port: {}", packet.protocol_version, packet.server_address, packet.server_port);

  switch (packet.intent) {
  case Intent::Status:
    state = State::Status;
    logger::debug("Setting state to Status");
    break;
  case Intent::Login:
    state = State::Login;
    logger::debug("Setting state to Login");
    break;
  }
}

void MinecraftClient::handle_status_request() {
  const std::string status =
      "{ \"version\": { \"name\": \"1.21.8\", \"protocol\": 340 }, \"players\": { \"max\": 20, \"online\": 1, \"sample\": [ { \"name\": "
      "\"Player\", \"id\": \"4566e69f-c907-48ee-8d71-d7ba5aa00d20\" } ] }, \"description\": { \"text\": \"Hello, world!\" }, \"favicon\": "
      "\"data:image/png;base64,<data>\", \"enforcesSecureChat\": false }";

  StatusResponse response_packet = {.json_status = status};

  send_packet(response_packet);
}

void MinecraftClient::handle_ping_request(ByteReader &reader) {
  PingRequest packet = PingRequest::read(reader);

  PingResponse response_packet = {.timestamp = packet.timestamp};

  send_packet(response_packet);
  disconnect_after_writing = true;
}

std::span<const uint8_t> MinecraftClient::get_outgoing() { return outgoing; }

void MinecraftClient::clear_outgoing() { outgoing.clear(); }
