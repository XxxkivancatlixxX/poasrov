#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <netinet/in.h>

enum ConnectionType {
    CONN_TCP,
    CONN_UDP,
    CONN_SERIAL_USB
};

class Connection {
public:
    Connection();
    virtual ~Connection();
    
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool is_connected() const = 0;
    virtual bool send(const uint8_t* data, uint16_t len) = 0;
    virtual bool receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) = 0;
    virtual const std::string& get_error() const { return m_error; }
    
protected:
    std::string m_error;
};

class TCPConnection : public Connection {
public:
    TCPConnection(const std::string& host, uint16_t port);
    ~TCPConnection();
    
    bool connect() override;
    void disconnect() override;
    bool is_connected() const override;
    bool send(const uint8_t* data, uint16_t len) override;
    bool receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) override;
    
    // Getters for creating ROV instance
    int get_socket() const { return m_socket; }
    sockaddr_in get_target_addr() const;
    
private:
    std::string m_host;
    uint16_t m_port;
    int m_socket;
    bool m_connected;
};

class UDPConnection : public Connection {
public:
    UDPConnection(const std::string& host, uint16_t port);
    ~UDPConnection();
    
    bool connect() override;
    void disconnect() override;
    bool is_connected() const override;
    bool send(const uint8_t* data, uint16_t len) override;
    bool receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) override;
    
    // Getters for creating ROV instance (UDP target)
    int get_socket() const { return m_socket; }
    sockaddr_in get_target_addr() const;
    
private:
    std::string m_host;
    uint16_t m_port;
    int m_socket;
    bool m_connected;
};

class SerialUSBConnection : public Connection {
public:
    SerialUSBConnection(const std::string& port, uint32_t baudrate);
    ~SerialUSBConnection();
    
    bool connect() override;
    void disconnect() override;
    bool is_connected() const override;
    bool send(const uint8_t* data, uint16_t len) override;
    bool receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) override;
    
private:
    std::string m_port;
    uint32_t m_baudrate;
    int m_serial_fd;
    bool m_connected;
};

// Detected protocol type
enum ProtocolType {
    PROTOCOL_UNKNOWN,
    PROTOCOL_MAVLINK,      // MAVProxy with MAVLink (Pixhawk)
    PROTOCOL_CUSTOM        // Custom firmware protocol
};

// Connection manager - creates and manages the appropriate connection type
class ConnectionManager {
public:
    ConnectionManager();
    ~ConnectionManager();
    
    bool create_tcp_connection(const std::string& host, uint16_t port);
    bool create_udp_connection(const std::string& host, uint16_t port);
    bool create_serial_connection(const std::string& port, uint32_t baudrate);
    
    bool connect();
    void disconnect();
    bool is_connected() const;
    bool send(const uint8_t* data, uint16_t len);
    bool receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len);
    const std::string& get_error() const;
    
    // Protocol detection
    ProtocolType get_protocol_type() const { return m_protocol; }
    bool detect_protocol();  // Auto-detect based on received data
    
    // Get underlying TCP/UDP connection for ROV (if applicable)
    TCPConnection* get_tcp_connection() const { 
        return (m_type == CONN_TCP) ? (TCPConnection*)m_connection : nullptr;
    }
    UDPConnection* get_udp_connection() const {
        return (m_type == CONN_UDP) ? (UDPConnection*)m_connection : nullptr;
    }
    
private:
    Connection* m_connection;
    ConnectionType m_type;
    ProtocolType m_protocol;
};
