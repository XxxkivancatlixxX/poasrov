// ConnectionTab.cpp
#include "ConnectionTab.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QLabel>

ConnectionTab::ConnectionTab(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    auto *group = new QGroupBox(tr("Pixhawk Connection"), this);
    auto *form  = new QFormLayout(group);

    m_typeGroup = new QButtonGroup(this);
    m_tcpRadio  = new QRadioButton(tr("TCP"), this);
    m_tcpRadio->setChecked(true);
    m_typeGroup->addButton(m_tcpRadio, 0);

    m_hostEdit = new QLineEdit("localhost", this);
    m_portSpin = new QSpinBox(this);
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(5760);

    form->addRow(tr("Connection Type:"), m_tcpRadio);
    form->addRow(tr("Host:"), m_hostEdit);
    form->addRow(tr("Port:"), m_portSpin);

    group->setLayout(form);

    m_connectBtn    = new QPushButton(tr("Connect"), this);
    m_disconnectBtn = new QPushButton(tr("Disconnect"), this);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_connectBtn);
    btnLayout->addWidget(m_disconnectBtn);
    btnLayout->addStretch();

    mainLayout->addWidget(group);
    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();

    connect(m_connectBtn, &QPushButton::clicked,
            this, &ConnectionTab::onConnectClicked);
    connect(m_disconnectBtn, &QPushButton::clicked,
            this, &ConnectionTab::onDisconnectClicked);
}

void ConnectionTab::onConnectClicked()
{
    const QString host = m_hostEdit->text();
    const quint16 port = static_cast<quint16>(m_portSpin->value());
    emit connectRequestedTCP(host, port);
}

void ConnectionTab::onDisconnectClicked()
{
    emit disconnectRequested();
}
