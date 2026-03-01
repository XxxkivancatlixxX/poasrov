#pragma once

#include <QObject>
#include <QTcpSocket>

class TelemetryClient : public QObject
{
    Q_OBJECT
public:
    explicit TelemetryClient(QObject *parent = nullptr);

    Q_INVOKABLE void connectToHost(const QString &host, quint16 port);
    Q_INVOKABLE void disconnectFromHost();
    bool isConnected() const;

    // Low-level send (used by ControlManager)
    bool sendRaw(const QByteArray &data);

signals:
    void connected();
    void disconnected();
    void telemetryPacketReceived(const QByteArray &packet);
    void errorOccurred(const QString &err);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError);

private:
    QTcpSocket m_socket;
    QByteArray m_buffer;
};
