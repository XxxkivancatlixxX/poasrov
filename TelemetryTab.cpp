// TelemetryTab.cpp
#include "TelemetryTab.h"
#include <QVBoxLayout>
#include <QLabel>

TelemetryTab::TelemetryTab(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    m_statusLabel   = new QLabel("Status: N/A", this);
    m_attitudeLabel = new QLabel("Attitude: N/A", this);
    m_depthLabel    = new QLabel("Depth: N/A", this);
    m_tempLabel     = new QLabel("Temperature: N/A", this);
    m_pressureLabel = new QLabel("Pressure: N/A", this);
    m_batteryLabel  = new QLabel("Battery: N/A", this);

    layout->addWidget(m_statusLabel);
    layout->addWidget(m_attitudeLabel);
    layout->addWidget(m_depthLabel);
    layout->addWidget(m_tempLabel);
    layout->addWidget(m_pressureLabel);
    layout->addWidget(m_batteryLabel);
    layout->addStretch();
}

void TelemetryTab::updateFromState(const RemoteRobotState &st)
{
    m_statusLabel->setText(
        QString("Status: %1 | Mode: %2")
            .arg(st.armed ? "ARMED" : "DISARMED")
            .arg(static_cast<int>(st.flight_mode))
    );

    m_attitudeLabel->setText(
        QString("Attitude: Roll=%1° Pitch=%2° Yaw=%3°")
            .arg(st.roll,  0, 'f', 1)
            .arg(st.pitch, 0, 'f', 1)
            .arg(st.yaw,   0, 'f', 1)
    );

    m_depthLabel->setText(
        QString("Depth: %1 m").arg(st.sensors.depth, 0, 'f', 2)
    );

    m_tempLabel->setText(
        QString("Temperature: %1 °C").arg(st.sensors.temperature, 0, 'f', 1)
    );

    m_pressureLabel->setText(
        QString("Pressure: %1 Pa").arg(st.sensors.pressure, 0, 'f', 1)
    );

    m_batteryLabel->setText(
        QString("Battery: %1 V, %2 A, %3 mAh (%4 %%)")
            .arg(st.battery.voltage,      0, 'f', 2)
            .arg(st.battery.current,      0, 'f', 2)
            .arg(st.battery.capacity_mah, 0, 'f', 0)
            .arg(static_cast<int>(st.battery.percentage))
    );
}
