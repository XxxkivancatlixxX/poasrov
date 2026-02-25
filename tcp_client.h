#pragma once

#include <cstdint>
#include <vector>
#include <string>

class TCPClient {
public:
    TCPClient();
    ~TCPClient();
    
    // Connect to server
    bool connect(const char* host, uint16_t port);
    
    // Disconnect from server
    void disconnect();
    
    // Check if connected
    bool is_connected() const;
    
    // Send data
    bool send(const uint8_t* data, uint16_t len);
    
    // Receive data (non-blocking)
    bool receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len);
    
    // Get connection state
    const std::string& get_error() const { return m_error; }
    
private:
    int m_socket;
    bool m_connected;
    std::string m_error;
};
