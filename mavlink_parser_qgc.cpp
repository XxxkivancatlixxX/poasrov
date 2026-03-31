// QGC-compatible MAVLink parser implementation
// Uses QGC's MAVLink headers instead of our own c_library_v2

#include "mavlink_parser.h"
#include <cstring>

// Include QGC's MAVLink - it's already in the include path
#include <mavlink.h>

MAVLinkParser::MAVLinkParser() {
    std::memset(&status, 0, sizeof(status));
}

bool MAVLinkParser::parse_byte(uint8_t byte, MAVLinkFrame& frame, MAVLinkTelemetry& telemetry) {
    mavlink_message_t msg;

    // Use the official MAVLink parser (handles v1/v2, CRC, etc.)
    if (!mavlink_parse_char(MAVLINK_COMM_0, byte, &msg, &status)) {
        return false;
    }

    // Fill simplified frame structure for callers that want raw info
    frame.start_byte   = msg.magic;
    frame.payload_len  = msg.len;
    frame.seq          = msg.seq;
    frame.sysid        = msg.sysid;
    frame.compid       = msg.compid;
    frame.msgid        = msg.msgid;
    std::memset(frame.payload, 0, sizeof(frame.payload));
    if (msg.len > 0) {
        const void* payload = _MAV_PAYLOAD(&msg);
        std::memcpy(frame.payload, payload, msg.len);
    }
    frame.checksum_low  = msg.ck[0];
    frame.checksum_high = msg.ck[1];

    // Update high‑level telemetry for selected messages
    switch (msg.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
            extract_heartbeat(msg, telemetry);
            break;
        case MAVLINK_MSG_ID_ATTITUDE:
            extract_attitude(msg, telemetry);
            break;
        case MAVLINK_MSG_ID_BATTERY_STATUS:
            extract_battery(msg, telemetry);
    