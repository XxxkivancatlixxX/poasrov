#include "tcp_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <cstdio>

TCPClient::TCPClient() : m_socket(-1), m_connected(false) {
}

TCPClient::~TCPClient() {
    disconnect();
}

bool TCPClient::connect(const char* host, uint16_t port) {
    if (m_connected) {
        disconnect();
    }
    
    // Create socket
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0) {
        m_error = "Failed to create socket";
        return false;
    }
    
    // Set socket to non-blocking
    int flags = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
    
    // Connect to server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);
    
    int result = ::connect(m_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (result < 0 && errno != EINPROGRESS) {
        m_error = "Failed to connect to server";
        close(m_socket);
        m_socket = -1;
        return false;
    }
    
    m_connected = true;
    m_error = "";
    return true;
}

void TCPClient::disconnect() {
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
    m_connected = false;
}

bool TCPClient::is_connected() const {
    return m_connected && m_socket >= 0;
}

bool TCPClient::send(const uint8_t* data, uint16_t len) {
    if (!is_connected()) {
        m_error = "Not connected";
        return false;
    }
    
    ssize_t sent = ::send(m_socket, data, len, MSG_DONTWAIT);
    if (sent < 0) {
        m_error = "Send failed";
        m_connected = false;
        return false;
    }
    
    return true;
}

bool TCPClient::receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) {
    if (!is_connected()) {
        received_len = 0;
        return false;
    }
    
    ssize_t received = ::recv(m_socket, buffer, buffer_size, MSG_DONTWAIT);
    if (received < 0) {
        received_len = 0;
        // EAGAIN/EWOULDBLOCK is OK for non-blocking socket
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            m_connected = false;
        }
        return false;
    }
    
    if (received == 0) {
        m_connected = false;
        received_len = 0;
        return false;
    }
    
    received_len = received;
    return true;
}
