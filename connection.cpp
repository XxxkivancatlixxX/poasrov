#include "connection.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <termios.h>

// ============== Connection Base Class ==============
Connection::Connection() {}
Connection::~Connection() {}

// ============== TCP Connection ==============
TCPConnection::TCPConnection(const std::string& host, uint16_t port)
    : m_host(host), m_port(port), m_socket(-1), m_connected(false) {}

TCPConnection::~TCPConnection() {
    disconnect();
}

bool TCPConnection::connect() {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0) {
        m_error = "Failed to create socket";
        return false;
    }
    
    int flags = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    inet_pton(AF_INET, m_host.c_str(), &addr.sin_addr);
    
    int result = ::connect(m_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (result < 0 && errno != EINPROGRESS) {
        m_error = "Failed to connect to " + m_host + ":" + std::to_string(m_port);
        close(m_socket);
        m_socket = -1;
        return false;
    }
    
    m_connected = true;
    m_error = "";
    return true;
}

void TCPConnection::disconnect() {
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
    m_connected = false;
}

bool TCPConnection::is_connected() const {
    return m_connected && m_socket >= 0;
}

bool TCPConnection::send(const uint8_t* data, uint16_t len) {
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

bool TCPConnection::receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) {
    if (!is_connected()) {
        received_len = 0;
        return false;
    }
    
    ssize_t received = ::recv(m_socket, buffer, buffer_size, MSG_DONTWAIT);
    if (received < 0) {
        received_len = 0;
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

// ============== UDP Connection ==============
UDPConnection::UDPConnection(const std::string& host, uint16_t port)
    : m_host(host), m_port(port), m_socket(-1), m_connected(false) {}

UDPConnection::~UDPConnection() {
    disconnect();
}

bool UDPConnection::connect() {
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        m_error = "Failed to create UDP socket";
        return false;
    }
    
    int flags = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    inet_pton(AF_INET, m_host.c_str(), &addr.sin_addr);
    
    if (::connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        m_error = "Failed to set UDP target";
        close(m_socket);
        m_socket = -1;
        return false;
    }
    
    m_connected = true;
    m_error = "";
    return true;
}

void UDPConnection::disconnect() {
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
    m_connected = false;
}

bool UDPConnection::is_connected() const {
    return m_connected && m_socket >= 0;
}

bool UDPConnection::send(const uint8_t* data, uint16_t len) {
    if (!is_connected()) {
        m_error = "Not connected";
        return false;
    }
    
    ssize_t sent = ::send(m_socket, data, len, MSG_DONTWAIT);
    if (sent < 0) {
        m_error = "UDP send failed";
        return false;
    }
    return true;
}

bool UDPConnection::receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) {
    if (!is_connected()) {
        received_len = 0;
        return false;
    }
    
    ssize_t received = ::recvfrom(m_socket, buffer, buffer_size, MSG_DONTWAIT, nullptr, nullptr);
    if (received < 0) {
        received_len = 0;
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            m_error = "UDP receive error";
        }
        return false;
    }
    
    received_len = received;
    return true;
}

// ============== Serial USB Connection ==============
SerialUSBConnection::SerialUSBConnection(const std::string& port, uint32_t baudrate)
    : m_port(port), m_baudrate(baudrate), m_serial_fd(-1), m_connected(false) {}

SerialUSBConnection::~SerialUSBConnection() {
    disconnect();
}

bool SerialUSBConnection::connect() {
    m_serial_fd = open(m_port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (m_serial_fd < 0) {
        m_error = "Cannot open " + m_port;
        return false;
    }
    
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    
    if (tcgetattr(m_serial_fd, &tty) != 0) {
        m_error = "tcgetattr failed";
        close(m_serial_fd);
        m_serial_fd = -1;
        return false;
    }
    
    speed_t speed;
    switch (m_baudrate) {
        case 9600:   speed = B9600; break;
        case 19200:  speed = B19200; break;
        case 38400:  speed = B38400; break;
        case 57600:  speed = B57600; break;
        case 115200: speed = B115200; break;
        default:     speed = B57600; break;
    }
    
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);
    
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    
    if (tcsetattr(m_serial_fd, TCSANOW, &tty) != 0) {
        m_error = "tcsetattr failed";
        close(m_serial_fd);
        m_serial_fd = -1;
        return false;
    }
    
    m_connected = true;
    m_error = "";
    return true;
}

void SerialUSBConnection::disconnect() {
    if (m_serial_fd >= 0) {
        close(m_serial_fd);
        m_serial_fd = -1;
    }
    m_connected = false;
}

bool SerialUSBConnection::is_connected() const {
    return m_connected && m_serial_fd >= 0;
}

bool SerialUSBConnection::send(const uint8_t* data, uint16_t len) {
    if (!is_connected()) {
        m_error = "Not connected";
        return false;
    }
    
    ssize_t written = write(m_serial_fd, data, len);
    if (written < 0) {
        m_error = "Serial write failed";
        return false;
    }
    return true;
}

bool SerialUSBConnection::receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) {
    if (!is_connected()) {
        received_len = 0;
        return false;
    }
    
    ssize_t n = read(m_serial_fd, buffer, buffer_size);
    if (n < 0) {
        received_len = 0;
        return false;
    }
    
    received_len = n;
    return true;
}

// ============== Connection Manager ==============
ConnectionManager::ConnectionManager() : m_connection(nullptr) {}

ConnectionManager::~ConnectionManager() {
    disconnect();
    if (m_connection) {
        delete m_connection;
        m_connection = nullptr;
    }
}

bool ConnectionManager::create_tcp_connection(const std::string& host, uint16_t port) {
    if (m_connection) {
        delete m_connection;
    }
    m_connection = new TCPConnection(host, port);
    m_type = CONN_TCP;
    return true;
}

bool ConnectionManager::create_udp_connection(const std::string& host, uint16_t port) {
    if (m_connection) {
        delete m_connection;
    }
    m_connection = new UDPConnection(host, port);
    m_type = CONN_UDP;
    return true;
}

bool ConnectionManager::create_serial_connection(const std::string& port, uint32_t baudrate) {
    if (m_connection) {
        delete m_connection;
    }
    m_connection = new SerialUSBConnection(port, baudrate);
    m_type = CONN_SERIAL_USB;
    return true;
}

bool ConnectionManager::connect() {
    if (!m_connection) return false;
    return m_connection->connect();
}

void ConnectionManager::disconnect() {
    if (m_connection) {
        m_connection->disconnect();
    }
}

bool ConnectionManager::is_connected() const {
    if (!m_connection) return false;
    return m_connection->is_connected();
}

bool ConnectionManager::send(const uint8_t* data, uint16_t len) {
    if (!m_connection) return false;
    return m_connection->send(data, len);
}

bool ConnectionManager::receive(uint8_t* buffer, uint16_t buffer_size, uint16_t& received_len) {
    if (!m_connection) {
        received_len = 0;
        return false;
    }
    return m_connection->receive(buffer, buffer_size, received_len);
}

const std::string& ConnectionManager::get_error() const {
    static const std::string no_conn = "No connection";
    if (!m_connection) return no_conn;
    return m_connection->get_error();
}
