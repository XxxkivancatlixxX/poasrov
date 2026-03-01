#pragma once

#include <QObject>
#include "telemetry_parser.h"
#include "telemetry_receiver.h"

// Combine TelemetryParser + TelemetryReceiver
// and expose updates via Qt signals.
class TelemetryReceiverQt : public QObject
{
    Q_OBJECT
public:
    explicit TelemetryReceiverQt(QObject *parent = nullptr);

    // Call this with raw bytes from TelemetryClient
    void handleRawPacket(const QByteArray &data);

    const RemoteRobotState &state() const { return m_receiver.get_state(); }

signals:
    void stateUpdated(const RemoteRobotState &state);

private:
    TelemetryParser   m_parser;
    TelemetryReceiver m_receiver;
};
