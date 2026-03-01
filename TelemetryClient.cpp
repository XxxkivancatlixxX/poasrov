#include "TelemetryClient.h"

TelemetryClient::TelemetryClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_socket, &QTcpSocket::readyRead,
            this, &TelemetryClient::onReadyRead);
    connect(&m_socket, &QTcpSocket::connected,
            this, &TelemetryClient::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected,
            this, &TelemetryClient::onDisconnected);
    connect(&m_socket,
            &QTcpSocket::errorOccurred,
            this, &TelemetryClient::onError);
}

void TelemetryClient::connectToHost(const QString &host, quint16 port)
{
    if (m_socket.state() != QAbstractSocket::UnconnectedState) {
        m_socket.abort();
    }
    m_socket.connectToHost(host, port);
}

void TelemetryClient::disconnectFromHost()
{
    m_socket.disconnectFromHost();
}

bool TelemetryClient::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

bool TelemetryClient::sendRaw(const QByteArray &data)
{
    if (!isConnected())
        return false;
    qint64 written = m_socket.write(data);
    return (written == data.size());
}

void TelemetryClient::onReadyRead()
{
    m_buffer.append(m_socket.readAll());

    const int packetSize = 512;  // TelemetryBridge packet size [17][14]
    while (m_buffer.size() >= packetSize) {
        QByteArray packet = m_buffer.left(packetSize);
        m_buffer.remove(0, packetSize);
        emit telemetryPacketReceived(packet);
    }
}

void TelemetryClient::onConnected()
{
    emit connected();
}

void TelemetryClient::onDisconnected()
{
    emit disconnected();
}

void TelemetryClient::onError(QAbstractSocket::SocketError)
{
    emit errorOccurred(m_socket.errorString());
}
