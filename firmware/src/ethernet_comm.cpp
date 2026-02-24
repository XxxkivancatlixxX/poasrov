#include "ethernet_comm.h"
#include <cstring>
#include <cstdio>

EthernetComm::EthernetComm() : socket_handle(-1), connected(false), rx_len(0) {
    std::memset(rx_buffer, 0, sizeof(rx_buffer));
}

EthernetComm::~EthernetComm() {
}

bool EthernetComm::init(const char* local_ip, uint16_t port) {
    printf("Initializing Ethernet on %s:%d\n", local_ip, port);
    connected = true;
    return true;
}

bool EthernetComm::send_data(const uint8_t* data, uint16_t len) {
    if (!connected) return false;
    printf("Sending %d bytes\n", len);
    return true;
}

bool EthernetComm::receive_data(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) {
    if (!connected) return false;
    received_len = 0;
    return true;
}

void EthernetComm::update() {
}

bool EthernetComm::is_connected() const {
    return connected;
}
