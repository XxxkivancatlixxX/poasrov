// MainWindow.cpp
#include "MainWindow.h"

#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>

#include "ConnectionTab.h"
#include "VideoWidget.h"
#include "VideoDecoder.h"

#include <SDL2/SDL.h>

// Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Initialize ControlSender defaults
    m_controlSender.set_armed(false);
    m_controlSender.set_flight_mode(0);

    // Initialize SDL for controller only
    if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    } else {
        input_init(); // will set ControllerState.connected if a pad is present [9]
    }

    setupUi();
    setupTimers();
}

// Destructor
MainWindow::~MainWindow()
{
    if (m_videoDecoder)
        m_videoDecoder->stop();

    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER);
}

// UI setup
void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *vbox = new QVBoxLayout(central);

    // Header
    auto *headerLayout = new QHBoxLayout;
    auto *titleLabel = new QLabel("ROV Control System - Qt", this);
    m_controllerLabel = new QLabel("Controller: DISCONNECTED", this);
    m_connectionLabel = new QLabel("Pixhawk: DISCONNECTED", this);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_controllerLabel);
    headerLayout->addSpacing(10);
    headerLayout->addWidget(m_connectionLabel);

    // Tabs
    m_tabs = new QTabWidget(this);

    m_connectionTab = new ConnectionTab(this);
    m_tabs->addTab(m_connectionTab, tr("Connection"));

    m_videoWidget  = new VideoWidget(this);
    m_videoDecoder = new VideoDecoder(this);

    auto *cameraTab = new QWidget(this);
    auto *camLayout = new QVBoxLayout(cameraTab);
    camLayout->addWidget(m_videoWidget);
    m_tabs->addTab(cameraTab, tr("Camera"));

    vbox->addLayout(headerLayout);
    vbox->addWidget(m_tabs);
    setCentralWidget(central);

    statusBar()->showMessage("Ready");

    // Connect connection tab
    connect(m_connectionTab, &ConnectionTab::connectRequestedTCP,
            this, &MainWindow::onConnectionRequestTCP);
    connect(m_connectionTab, &ConnectionTab::disconnectRequested,
            this, &MainWindow::onDisconnectRequested);

    // Video frames → widget
    connect(m_videoDecoder, &VideoDecoder::frameReady,
            m_videoWidget, &VideoWidget::setFrame);

    // Optionally start video:
    // m_videoDecoder->start("rtsp://192.168.1.2:8554/cam");
}

void MainWindow::setupTimers()
{
    // Telemetry + control at ~60 Hz
    m_pollTimer.setInterval(16);
    connect(&m_pollTimer, &QTimer::timeout,
            this, &MainWindow::onPeriodicUpdate);
    m_pollTimer.start();

    // Controller polling
    m_controllerTimer.setInterval(16);
    connect(&m_controllerTimer, &QTimer::timeout,
            this, &MainWindow::onControllerPolled);
    m_controllerTimer.start();
}

// Called every ~16 ms
void MainWindow::onPeriodicUpdate()
{
    processTelemetry();
    sendControlPacket();
    updateConnectionStatusLabel();
}

// Called every ~16 ms
void MainWindow::onControllerPolled()
{
    input_update();                    // read SDL axes/triggers/buttons [9]
    m_lastController = input_get_state();

    if (m_lastController.connected) {
        m_controllerLabel->setText("Controller: CONNECTED");
    } else {
        m_controllerLabel->setText("Controller: DISCONNECTED");
    }
}

// Connection tab → ConnectionManager
void MainWindow::onConnectionRequestTCP(const QString &host, quint16 port)
{
    const std::string hostStr = host.toStdString();

    if (!m_connection.create_tcp_connection(hostStr, port)) { // [3]
        statusBar()->showMessage("Failed to create TCP connection");
        return;
    }

    if (!m_connection.connect()) {
        statusBar()->showMessage(
            QString("Connect failed: %1")
                .arg(QString::fromStdString(m_connection.get_error())));
        return;
    }

    statusBar()->showMessage(
        QString("Connected to %1:%2").arg(host).arg(port));
    updateConnectionStatusLabel();
}

void MainWindow::onDisconnectRequested()
{
    m_connection.disconnect();
    statusBar()->showMessage("Disconnected");
    updateConnectionStatusLabel();
}

void MainWindow::updateConnectionStatusLabel()
{
    if (m_connection.is_connected()) {
        m_connectionLabel->setText("Pixhawk: CONNECTED");
    } else {
        m_connectionLabel->setText("Pixhawk: DISCONNECTED");
    }
}

// Telemetry: read from ConnectionManager, parse, store
void MainWindow::processTelemetry()
{
    if (!m_connection.is_connected())
        return;

    uint8_t buffer[1024];
    uint16_t recvLen = 0;

    if (!m_connection.receive(buffer, sizeof(buffer), recvLen) || recvLen == 0) {
        return; // no data
    }

    TelemetryPacket pkt{};
    if (!m_telemetryParser.parse_packet(buffer, recvLen, pkt)) { // [18][19]
        return; // not a telemetry packet
    }

    // Map to RemoteTelemetryPacket for TelemetryReceiver [19][21]
    RemoteTelemetryPacket remote{};
    remote.packet_type = pkt.packet_type;
    remote.state.armed       = pkt.state.armed;
    remote.state.flight_mode = pkt.state.flight_mode;

    // Sensors
    remote.state.sensors.gyro_x      = pkt.state.sensors.gyro_x;
    remote.state.sensors.gyro_y      = pkt.state.sensors.gyro_y;
    remote.state.sensors.gyro_z      = pkt.state.sensors.gyro_z;
    remote.state.sensors.accel_x     = pkt.state.sensors.accel_x;
    remote.state.sensors.accel_y     = pkt.state.sensors.accel_y;
    remote.state.sensors.accel_z     = pkt.state.sensors.accel_z;
    remote.state.sensors.mag_x       = pkt.state.sensors.mag_x;
    remote.state.sensors.mag_y       = pkt.state.sensors.mag_y;
    remote.state.sensors.mag_z       = pkt.state.sensors.mag_z;
    remote.state.sensors.depth       = pkt.state.sensors.depth;
    remote.state.sensors.temperature = pkt.state.sensors.temperature;
    remote.state.sensors.pressure    = pkt.state.sensors.pressure;

    // Battery
    remote.state.battery.voltage      = pkt.state.battery.voltage;
    remote.state.battery.current      = pkt.state.battery.current;
    remote.state.battery.capacity_mah = pkt.state.battery.capacity_mah;
    remote.state.battery.percentage   = pkt.state.battery.percentage;

    // Camera
    remote.state.camera.camera_type   = pkt.state.camera.camera_type;
    remote.state.camera.servo_min_pwm = pkt.state.camera.servo_min_pwm;
    remote.state.camera.servo_max_pwm = pkt.state.camera.servo_max_pwm;
    remote.state.camera.gimbal_type   = pkt.state.camera.gimbal_type;

    // Water
    remote.state.water.pressure_offset = pkt.state.water.pressure_offset;
    remote.state.water.temp_offset     = pkt.state.water.temp_offset;
    remote.state.water.salinity_type   = pkt.state.water.salinity_type;

    // PID tuning
    remote.state.pid_tuning.roll_p   = pkt.state.pid_tuning.roll_p;
    remote.state.pid_tuning.roll_i   = pkt.state.pid_tuning.roll_i;
    remote.state.pid_tuning.roll_d   = pkt.state.pid_tuning.roll_d;
    remote.state.pid_tuning.pitch_p  = pkt.state.pid_tuning.pitch_p;
    remote.state.pid_tuning.pitch_i  = pkt.state.pid_tuning.pitch_i;
    remote.state.pid_tuning.pitch_d  = pkt.state.pid_tuning.pitch_d;
    remote.state.pid_tuning.yaw_p    = pkt.state.pid_tuning.yaw_p;
    remote.state.pid_tuning.yaw_i    = pkt.state.pid_tuning.yaw_i;
    remote.state.pid_tuning.yaw_d    = pkt.state.pid_tuning.yaw_d;
    remote.state.pid_tuning.depth_p  = pkt.state.pid_tuning.depth_p;
    remote.state.pid_tuning.depth_i  = pkt.state.pid_tuning.depth_i;
    remote.state.pid_tuning.depth_d  = pkt.state.pid_tuning.depth_d;

    // Orientation
    remote.state.roll  = pkt.state.roll;
    remote.state.pitch = pkt.state.pitch;
    remote.state.yaw   = pkt.state.yaw;

    m_telemetryReceiver.update_from_packet(remote); // [20]

    const RemoteRobotState &st = m_telemetryReceiver.get_state();
    // Example: reflect depth in title
    setWindowTitle(QString("ROV GUI - Depth %1 m")
                       .arg(st.sensors.depth, 0, 'f', 2));
}

// Control: build ControlPacket from controller state and send
void MainWindow::sendControlPacket()
{
    if (!m_connection.is_connected())
        return;

    // Use existing logic: right trigger as throttle, etc. [4]
    m_controlSender.set_control_mode(m_lastController);

    // You can later wire these from UI controls
    // m_controlSender.set_armed(true/false);
    // m_controlSender.set_flight_mode(modeIndex);

    std::vector<uint8_t> bytes = m_controlSender.serialize(); // adds checksum [4][5]
    if (!bytes.empty()) {
        m_connection.send(bytes.data(),
                          static_cast<uint16_t>(bytes.size())); // [3]
    }
}
