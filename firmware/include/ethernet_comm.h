#pragma once

#include <cstdint>
#include <vector>

class EthernetComm {
public:
    EthernetComm();
    ~EthernetComm();
    
    bool init(const char* local_ip, uint16_t port);
    bool send_data(const uint8_t* data, uint16_t len);
    bool receive_data(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len);
    void update();
    bool is_connected() const;
    
private:
    int socket_handle;
    bool connected;
    uint8_t rx_buffer[2048];
    uint16_t rx_len;
};
