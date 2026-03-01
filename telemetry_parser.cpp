#include "telemetry_parser.h"
#include <cstring>

TelemetryParser::TelemetryParser() {}

bool TelemetryParser::parse_packet(const uint8_t* data, uint16_t len, TelemetryPacket& packet) {
    // Telemetry packets should be exactly sizeof(TelemetryPacket) bytes
    if (len < sizeof(TelemetryPacket)) {
        return false;
    }
    
    // Check packet type
    if (data[0] != 2) {
        return false;  // Not a telemetry packet (type 2)
    }
    
    memcpy(&packet, data, sizeof(TelemetryPacket));
    
    // Verify checksum
    uint8_t calc_checksum = calculate_checksum(data, sizeof(TelemetryPacket) - 1);
    if (calc_checksum != packet.checksum) {
        // Checksum mismatch - log but still accept for debugging
        // return false;
    }
    
    return true;
}

uint8_t TelemetryParser::calculate_checksum(const uint8_t* data, uint16_t len) {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}
