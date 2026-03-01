#pragma once

#include <QObject>
#include <QTimer>
#include "input.h"
#include "control_sender.h"

class TelemetryClient;

class ControlManager : public QObject
{
    Q_OBJECT
public:
    ControlManager(TelemetryClient *client, QObject *parent = nullptr);

public slots:
    void setControllerState(const ControllerState &st);

private slots:
    void onTimer();

private:
    TelemetryClient *m_client;
    ControlSender    m_sender;
    ControllerState  m_currentState;
    QTimer           m_timer;
};
