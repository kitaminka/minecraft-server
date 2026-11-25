#pragma once

#include "protocol/byte_reader.hpp"
#include "protocol/byte_writer.hpp"
#include <cstdint>
#include <span>
#include <vector>

enum class State { Handshaking, Status, Login, Play };

class MinecraftClient {
public:
  int handle_packets(std::span<const uint8_t> buffer);

  std::span<const uint8_t> get_outgoing();

  void clear_outgoing();

  bool disconnect_after_writing{false};

private:
  State state{State::Handshaking};

  std::vector<uint8_t> outgoing;

  int handle_packet(ByteReader &reader);
  void handle_packet_data(int packet_id, ByteReader &reader);

  void handshaking_handler(int packet_id, ByteReader &reader);
  void status_handler(int packet_id, ByteReader &reader);
  void login_handler(int packet_id, ByteReader &reader);
  void play_handler(int packet_id, ByteReader &reader);

  void handle_intention(ByteReader &reader);

  void handle_status_request();
  void handle_ping_request(ByteReader &reader);

  void handle_login_start(ByteReader& reader);

  template <typename ClientBoundPacket> void send_packet(const ClientBoundPacket &packet) {
    std::vector<uint8_t> payload;
    ByteWriter payload_writer(payload);
    packet.write(payload_writer);

    int id_size = ByteWriter::get_varint_size(packet.ID);

    ByteWriter writer(outgoing);
    writer.write_varint(payload.size() + id_size);
    writer.write_varint(packet.ID);
    outgoing.insert(outgoing.end(), payload.begin(), payload.end());
  }
};
