#include "mavlink_handler.h"

ProtocolHandler::ProtocolHandler() : sequence_counter(0) {}

ProtocolHandler::~ProtocolHandler() {}

bool ProtocolHandler::init() {
    return true;
}

void ProtocolHandler::parse_control_packet(const uint8_t* data, uint16_t len) {
    if (len < sizeof(ControlPacket)) return;
}

TelemetryPacket ProtocolHandler::create_telemetry_packet(const RobotState& state) {
    TelemetryPacket pkt;
    pkt.packet_type = 2;
    pkt.state = state;
    pkt.checksum = calculate_checksum((uint8_t*)&pkt, sizeof(pkt) - 1);
    return pkt;
}

uint8_t ProtocolHandler::calculate_checksum(const uint8_t* data, uint16_t len) {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}
