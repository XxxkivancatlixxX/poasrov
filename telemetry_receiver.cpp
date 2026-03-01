#include "telemetry_receiver.h"
#include <cstring>

TelemetryReceiver::TelemetryReceiver() {
    std::memset(&latest_state, 0, sizeof(latest_state));
}

TelemetryReceiver::~TelemetryReceiver() {}

void TelemetryReceiver::update_from_packet(const RemoteTelemetryPacket& pkt) {
    if (pkt.packet_type == 2) {
        latest_state = pkt.state;
    }
}
