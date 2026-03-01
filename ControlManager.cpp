#include "ControlManager.h"
#include "TelemetryClient.h"

ControlManager::ControlManager(TelemetryClient *client, QObject *parent)
    : QObject(parent),
      m_client(client)
{
    m_timer.setInterval(20); // ~50 Hz (matches SDL version) [11]
    connect(&m_timer, &QTimer::timeout,
            this, &ControlManager::onTimer);
    m_timer.start();

    // Default: disarmed, mode 0
    m_sender.set_armed(false);
    m_sender.set_flight_mode(0);
}

void ControlManager::setControllerState(const ControllerState &st)
{
    m_currentState = st;
}

void ControlManager::onTimer()
{
    if (!m_client || !m_client->isConnected())
        return;

    // Normal control mode based on controller state
    m_sender.set_control_mode(m_currentState);  // uses trigger_right throttle [4]

    std::vector<uint8_t> bytes = m_sender.serialize();  // adds checksum [4][5]
    QByteArray data(reinterpret_cast<const char*>(bytes.data()),
                    static_cast<int>(bytes.size()));
    m_client->sendRaw(data);
}
