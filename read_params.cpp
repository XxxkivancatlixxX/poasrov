#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "libs/c_library_v2/common/mavlink.h"

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in target;
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port = htons(5760);
    inet_pton(AF_INET, "192.168.1.2", &target.sin_addr);
    
    // Send heartbeat first
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    
    mavlink_msg_heartbeat_pack(255, 190, &msg, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, 0, 0, MAV_STATE_ACTIVE);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    sendto(sock, buf, len, 0, (struct sockaddr*)&target, sizeof(target));
    
    printf("Sent heartbeat, waiting 1 second...\n");
    sleep(1);
    
    // Request FRAME_TYPE parameter
    mavlink_param_request_read_t req;
    req.target_system = 1;
    req.target_component = 1;
    strncpy(req.param_id, "FRAME_TYPE", 16);
    req.param_index = -1;
    
    mavlink_msg_param_request_read_encode(255, 190, &msg, &req);
    len = mavlink_msg_to_send_buffer(buf, &msg);
    sendto(sock, buf, len, 0, (struct sockaddr*)&target, sizeof(target));
    
    printf("Requested FRAME_TYPE parameter\n");
    
    // Listen for response
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    
    for (int i = 0; i < 50; i++) {
        int n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
        if (n > 0) {
            mavlink_message_t msg;
            mavlink_status_t status;
            
            for (int j = 0; j < n; j++) {
                if (mavlink_parse_char(MAVLINK_COMM_0, buf[j], &msg, &status)) {
                    if (msg.msgid == MAVLINK_MSG_ID_PARAM_VALUE) {
                        mavlink_param_value_t param;
                        mavlink_msg_param_value_decode(&msg, &param);
                        printf("Parameter: %s = %.2f (type=%d)\n", param.param_id, param.param_value, param.param_type);
                    }
                }
            }
        }
        usleep(100000); // 100ms
    }
    
    close(sock);
    return 0;
}
