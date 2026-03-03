// ConnectionTab.h
#pragma once

#include <QWidget>

class QLineEdit;
class QSpinBox;
class QButtonGroup;
class QRadioButton;
class QPushButton;

class ConnectionTab : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionTab(QWidget *parent = nullptr);

signals:
    void connectRequestedTCP(const QString &host, quint16 port);
    void disconnectRequested();

private slots:
    void onConnectClicked();
    void onDisconnectClicked();

private:
    QButtonGroup  *m_typeGroup = nullptr;
    QRadioButton  *m_tcpRadio  = nullptr;
    QLineEdit     *m_hostEdit  = nullptr;
    QSpinBox      *m_portSpin  = nullptr;
    QPushButton   *m_connectBtn = nullptr;
    QPushButton   *m_disconnectBtn = nullptr;
};
