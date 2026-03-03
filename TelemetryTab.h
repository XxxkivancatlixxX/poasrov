// TelemetryTab.h
#pragma once

#include <QWidget>
#include "telemetry_receiver.h"

class QLabel;

class TelemetryTab : public QWidget
{
    Q_OBJECT
public:
    explicit TelemetryTab(QWidget *parent = nullptr);

public slots:
    void updateFromState(const RemoteRobotState &st);

private:
    QLabel *m_statusLabel;
    QLabel *m_attitudeLabel;
    QLabel *m_depthLabel;
    QLabel *m_tempLabel;
    QLabel *m_pressureLabel;
    QLabel *m_batteryLabel;
};
