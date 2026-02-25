#include "ui.h"
#include "control_sender.h"
#include "connection.h"
#include "telemetry_parser.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <vector>
#include <string>
#include <ctime>

static SDL_Window   *g_window   = NULL;
static SDL_Renderer *g_renderer = NULL;

// Control sender for communicating with firmware
static ControlSender g_control_sender;

// Connection manager for all connection types
static ConnectionManager g_connection;

// Telemetry parser
static TelemetryParser g_telemetry_parser;

static bool g_armed = false;
static int g_selected_tab = 0;
static float g_motor_test[8] = {0};
static std::vector<std::string> g_log_messages;
static const int MAX_LOG_MESSAGES = 100;

static struct {
    float gyro_x_offset = 0.0f, gyro_y_offset = 0.0f, gyro_z_offset = 0.0f;
    float accel_x_scale = 1.0f, accel_y_scale = 1.0f, accel_z_scale = 1.0f;
    float compass_declination = 0.0f;
    int compass_rotation = 0;
} sensor_params;

static struct {
    int flight_mode = 0;
    float failsafe_throttle = 0.0f;
    float failsafe_depth = 50.0f;
    int failsafe_action = 0;
} safety_params;

static struct {
    float battery_voltage = 12.0f;
    float battery_capacity = 18000.0f;
    float battery_current = 0.0f;
} battery_params;

static struct {
    float pid_roll_p = 0.15f, pid_roll_i = 0.02f, pid_roll_d = 0.04f;
    float pid_pitch_p = 0.15f, pid_pitch_i = 0.02f, pid_pitch_d = 0.04f;
    float pid_yaw_p = 0.2f, pid_yaw_i = 0.05f, pid_yaw_d = 0.0f;
    float pid_depth_p = 2.5f, pid_depth_i = 0.1f, pid_depth_d = 0.5f;
} pid_params;

static struct {
    int camera_type = 0;
    float camera_servo_min = 1100.0f;
    float camera_servo_max = 1900.0f;
    int camera_gimbal_type = 0;
} camera_params;

static struct {
    int pressure_offset = 0;
    float temp_sensor_offset = 0.0f;
    int salinity_type = 0;
} water_params;

// Real telemetry data from Pixhawk
static struct {
    float battery_voltage = 0.0f;
    float battery_current = 0.0f;
    int battery_percentage = 0;
    float gyro_x = 0.0f, gyro_y = 0.0f, gyro_z = 0.0f;
    float accel_x = 0.0f, accel_y = 0.0f, accel_z = 0.0f;
    float mag_x = 0.0f, mag_y = 0.0f, mag_z = 0.0f;
    float depth = 0.0f;
    float temperature = 0.0f;
    float pressure = 0.0f;
    float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
    uint8_t armed = 0;
    uint8_t flight_mode = 0;
} telemetry_data;

// Connection settings
static struct {
    int connection_type = 0;  // 0=TCP, 1=UDP, 2=Serial
    char tcp_host[128] = "192.168.1.2";
    int tcp_port = 5760;
    char udp_host[128] = "192.168.1.2";
    int udp_port = 5760;
    char serial_port[128] = "/dev/ttyACM0";
    int serial_baudrate = 2;  // Index: 0=9600, 1=19200, 2=57600, 3=115200
    bool trying_connect = false;
} connection_settings;

void ui_init(SDL_Window *window, SDL_Renderer *renderer)
{
    g_window = window;
    g_renderer = renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowPadding = ImVec2(16, 16);
    style.FramePadding = ImVec2(8, 6);
    style.ItemSpacing = ImVec2(12, 8);
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.0f, 0.50f, 1.0f, 0.80f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.60f, 1.0f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.40f, 0.85f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.60f, 1.0f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void ui_process_event(const SDL_Event &e)
{
    ImGui_ImplSDL2_ProcessEvent(&e);
}

void ui_new_frame(void)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ui_draw(const ControllerState &ctrl, SDL_Texture *video_tex)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    
    ImGui::Begin("##MainDockSpace", nullptr, 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    ImGui::SetCursorPos(ImVec2(10, 5));
    ImGui::Text("ROV Control System - QGC Clone");
    ImGui::SameLine(ImGui::GetWindowWidth() - 250);
    
    if (ctrl.connected) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Controller: CONNECTED");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Controller: DISCONNECTED");
    }
    ImGui::Separator();
    
    if (ImGui::BeginTabBar("##MainTabs", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("Connection")) {
            ImGui::Text("PIXHAWK CONNECTION");
            ImGui::Separator();
            
            ImGui::Text("Connection Type:");
            ImGui::RadioButton("TCP##conntype", &connection_settings.connection_type, 0);
            ImGui::SameLine();
            ImGui::RadioButton("UDP##conntype", &connection_settings.connection_type, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Serial USB##conntype", &connection_settings.connection_type, 2);
            
            ImGui::Separator();
            
            if (connection_settings.connection_type == 0) {
                ImGui::Text("TCP Settings:");
                ImGui::InputText("Host##tcp", connection_settings.tcp_host, sizeof(connection_settings.tcp_host));
                ImGui::InputInt("Port##tcp", &connection_settings.tcp_port);
            } else if (connection_settings.connection_type == 1) {
                ImGui::Text("UDP Settings:");
                ImGui::InputText("Host##udp", connection_settings.udp_host, sizeof(connection_settings.udp_host));
                ImGui::InputInt("Port##udp", &connection_settings.udp_port);
            } else {
                ImGui::Text("Serial USB Settings:");
                ImGui::InputText("Port##serial", connection_settings.serial_port, sizeof(connection_settings.serial_port));
                const char* baudrates[] = {"9600", "19200", "57600", "115200"};
                ImGui::Combo("Baud Rate##serial", &connection_settings.serial_baudrate, baudrates, 4);
            }
            
            ImGui::Separator();
            
            if (g_connection.is_connected()) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: CONNECTED");
                if (ImGui::Button("Disconnect", ImVec2(120, 30))) {
                    g_connection.disconnect();
                    ui_log("Disconnected");
                }
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Status: DISCONNECTED");
                if (ImGui::Button("Connect", ImVec2(120, 30))) {
                    uint32_t baudrates[] = {9600, 19200, 57600, 115200};
                    
                    if (connection_settings.connection_type == 0) {
                        g_connection.create_tcp_connection(connection_settings.tcp_host, connection_settings.tcp_port);
                    } else if (connection_settings.connection_type == 1) {
                        g_connection.create_udp_connection(connection_settings.udp_host, connection_settings.udp_port);
                    } else {
                        g_connection.create_serial_connection(connection_settings.serial_port, 
                            baudrates[connection_settings.serial_baudrate]);
                    }
                    
                    if (g_connection.connect()) {
                        ui_log("Connected successfully");
                    } else {
                        std::string msg = "Connection failed: " + g_connection.get_error();
                        ui_log(msg.c_str());
                    }
                }
            }
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Flight")) {
            float available_height = ImGui::GetContentRegionAvail().y;
            float available_width = ImGui::GetContentRegionAvail().x;
            
            ImGui::BeginChild("VideoPanel", ImVec2(available_width * 0.72f, available_height * 0.6f), false);
            ImGui::Text("CAMERA FEED");
            ImGui::Separator();
            if (video_tex) {
                int w, h;
                SDL_QueryTexture(video_tex, NULL, NULL, &w, &h);
                float panel_width = ImGui::GetContentRegionAvail().x - 10;
                float panel_height = ImGui::GetContentRegionAvail().y - 10;
                ImVec2 display_size = ImVec2(panel_width, panel_height);
                ImGui::Image((ImTextureID)video_tex, display_size, ImVec2(0,0), ImVec2(1,1));
            } else {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No video feed");
            }
            ImGui::EndChild();
            
            ImGui::SameLine();
            
            ImGui::BeginChild("ControlPanel", ImVec2(available_width * 0.27f, available_height * 0.6f), false);
            
            // Connection status
            if (g_connection.is_connected()) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "CONNECTED TO PIXHAWK");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "DISCONNECTED - Go to Connection tab");
            }
            ImGui::Separator();
            
            // ARM button - disabled if not connected
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
            bool can_arm = g_connection.is_connected();
            
            if (!can_arm) {
                ImGui::BeginDisabled();
            }
            
            if (!g_armed) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
                if (ImGui::Button("ARM SYSTEM", ImVec2(-1, 40))) {
                    g_armed = true;
                    g_control_sender.set_armed(true);
                }
                ImGui::PopStyleColor(2);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                if (ImGui::Button("DISARM SYSTEM", ImVec2(-1, 40))) {
                    g_armed = false;
                    g_control_sender.set_armed(false);
                }
                ImGui::PopStyleColor(2);
            }
            
            if (!can_arm) {
                ImGui::EndDisabled();
            }
            ImGui::PopStyleVar();
            
            ImGui::TextColored(
                g_armed ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.8f, 0.8f, 0.8f, 1.0f),
                g_armed ? "STATUS: ARMED" : "STATUS: DISARMED"
            );
            ImGui::Separator();
            
            ImGui::Text("CONTROLLER STATE");
            if (ctrl.connected) {
                ImGui::ProgressBar(ctrl.axis_left_x * 0.5f + 0.5f, ImVec2(-1, 20), "Left X");
                ImGui::ProgressBar(ctrl.axis_left_y * 0.5f + 0.5f, ImVec2(-1, 20), "Left Y");
                ImGui::ProgressBar(ctrl.axis_right_x * 0.5f + 0.5f, ImVec2(-1, 20), "Right X");
                ImGui::ProgressBar(ctrl.axis_right_y * 0.5f + 0.5f, ImVec2(-1, 20), "Right Y");
                ImGui::ProgressBar(ctrl.trigger_left, ImVec2(-1, 20), "L Trigger");
                ImGui::ProgressBar(ctrl.trigger_right, ImVec2(-1, 20), "R Trigger");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Not Connected");
            }
            ImGui::EndChild();
            
            ImGui::BeginChild("MotorPanel", ImVec2(available_width, available_height * 0.39f), false);
            ImGui::Text("SENSORS & STATUS");
            ImGui::Separator();
            
            // Telemetry data
            ImGui::Text("Depth: %.2f m | Temp: %.1f°C | Battery: %.2f V (%.0f%%)", 
                telemetry_data.depth, telemetry_data.temperature, 
                telemetry_data.battery_voltage, (float)telemetry_data.battery_percentage);
            
            ImGui::Text("Attitude: Roll=%.1f° Pitch=%.1f° Yaw=%.1f°", 
                telemetry_data.roll, telemetry_data.pitch, telemetry_data.yaw);
            
            ImGui::Text("Gyro: X=%.2f Y=%.2f Z=%.2f", 
                telemetry_data.gyro_x, telemetry_data.gyro_y, telemetry_data.gyro_z);
            
            ImGui::Text("Accel: X=%.2f Y=%.2f Z=%.2f", 
                telemetry_data.accel_x, telemetry_data.accel_y, telemetry_data.accel_z);
            
            ImGui::Separator();
            ImGui::Text("MOTOR CONTROLS");
            float slider_width = ImGui::GetContentRegionAvail().x / 2.0f - 5;
            for (int i = 0; i < 8; ++i) {
                char label[32];
                snprintf(label, sizeof(label), "M%d", i + 1);
                if (i % 2 != 0) ImGui::SameLine();
                ImGui::PushItemWidth(slider_width);
                ImGui::SliderFloat(label, &g_motor_test[i], 0.0f, 1.0f, "%.2f");
                ImGui::PopItemWidth();
            }
            ImGui::Separator();
            ImGui::Text("PIXHAWK LOG");
            ImGui::BeginChild("LogWindow", ImVec2(0, 0), true);
            for (const auto& msg : g_log_messages) {
                ImGui::TextWrapped("%s", msg.c_str());
            }
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();
            ImGui::EndChild();
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Setup")) {
            ImGui::Text("VEHICLE SETUP");
            ImGui::Separator();
            
            ImGui::Text("Vehicle Type: Submarine (ROV)");
            ImGui::Text("Flight Controller: Pixhawk 2.4.8");
            ImGui::Text("Frame Type: Vectored");
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Tuning")) {
            ImGui::Text("PID TUNING");
            ImGui::Separator();
            
            ImGui::Text("Roll PID:");
            ImGui::SliderFloat("##roll_p", &pid_params.pid_roll_p, 0.0f, 1.0f);
            ImGui::SliderFloat("##roll_i", &pid_params.pid_roll_i, 0.0f, 0.5f);
            ImGui::SliderFloat("##roll_d", &pid_params.pid_roll_d, 0.0f, 0.5f);
            
            ImGui::Text("Pitch PID:");
            ImGui::SliderFloat("##pitch_p", &pid_params.pid_pitch_p, 0.0f, 1.0f);
            ImGui::SliderFloat("##pitch_i", &pid_params.pid_pitch_i, 0.0f, 0.5f);
            ImGui::SliderFloat("##pitch_d", &pid_params.pid_pitch_d, 0.0f, 0.5f);
            
            ImGui::Text("Yaw PID:");
            ImGui::SliderFloat("##yaw_p", &pid_params.pid_yaw_p, 0.0f, 1.0f);
            ImGui::SliderFloat("##yaw_i", &pid_params.pid_yaw_i, 0.0f, 0.5f);
            ImGui::SliderFloat("##yaw_d", &pid_params.pid_yaw_d, 0.0f, 0.5f);
            
            ImGui::Text("Depth PID:");
            ImGui::SliderFloat("##depth_p", &pid_params.pid_depth_p, 0.0f, 5.0f);
            ImGui::SliderFloat("##depth_i", &pid_params.pid_depth_i, 0.0f, 1.0f);
            ImGui::SliderFloat("##depth_d", &pid_params.pid_depth_d, 0.0f, 2.0f);
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Sensors")) {
            ImGui::Text("SENSOR CALIBRATION");
            ImGui::Separator();
            
            ImGui::Text("Gyroscope Calibration:");
            ImGui::SliderFloat("Gyro X Offset", &sensor_params.gyro_x_offset, -1.0f, 1.0f);
            ImGui::SliderFloat("Gyro Y Offset", &sensor_params.gyro_y_offset, -1.0f, 1.0f);
            ImGui::SliderFloat("Gyro Z Offset", &sensor_params.gyro_z_offset, -1.0f, 1.0f);
            
            ImGui::Text("Accelerometer Calibration:");
            ImGui::SliderFloat("Accel X Scale", &sensor_params.accel_x_scale, 0.5f, 2.0f);
            ImGui::SliderFloat("Accel Y Scale", &sensor_params.accel_y_scale, 0.5f, 2.0f);
            ImGui::SliderFloat("Accel Z Scale", &sensor_params.accel_z_scale, 0.5f, 2.0f);
            
            ImGui::Text("Compass Calibration:");
            ImGui::SliderFloat("Declination", &sensor_params.compass_declination, -180.0f, 180.0f);
            ImGui::Combo("Compass Rotation", &sensor_params.compass_rotation, 
                "ROTATION_NONE\0ROTATION_YAW_45\0ROTATION_YAW_90\0ROTATION_YAW_135\0");
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Safety")) {
            ImGui::Text("SAFETY SETTINGS");
            ImGui::Separator();
            
            ImGui::Text("Flight Mode:");
            ImGui::Combo("##flight_mode", &safety_params.flight_mode,
                "STABILIZE\0ACRO\0ALT_HOLD\0AUTO\0GUIDED\0");
            
            ImGui::Text("Failsafe Throttle:");
            ImGui::SliderFloat("##failsafe_throttle", &safety_params.failsafe_throttle, 0.0f, 1.0f);
            
            ImGui::Text("Failsafe Depth:");
            ImGui::SliderFloat("##failsafe_depth", &safety_params.failsafe_depth, 0.0f, 300.0f);
            
            ImGui::Text("Failsafe Action:");
            ImGui::Combo("##failsafe_action", &safety_params.failsafe_action,
                "NOTHING\0SURFACE\0ENTER_GUIDED\0HOLD\0");
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Battery")) {
            ImGui::Text("BATTERY STATUS");
            ImGui::Separator();
            
            ImGui::Text("Battery Voltage:");
            ImGui::ProgressBar(telemetry_data.battery_voltage / 16.0f, ImVec2(-1, 20));
            ImGui::Text("%.2f V", telemetry_data.battery_voltage);
            
            ImGui::Text("Battery Percentage:");
            ImGui::ProgressBar(telemetry_data.battery_percentage / 100.0f, ImVec2(-1, 20));
            ImGui::Text("%d %%", telemetry_data.battery_percentage);
            
            ImGui::Text("Battery Current:");
            ImGui::ProgressBar(telemetry_data.battery_current / 50.0f, ImVec2(-1, 20));
            ImGui::Text("%.2f A", telemetry_data.battery_current);
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Camera")) {
            ImGui::Text("CAMERA SETTINGS");
            ImGui::Separator();
            
            ImGui::Text("Camera Type:");
            ImGui::Combo("##camera_type", &camera_params.camera_type,
                "RUNCAM\0GOPRO\0GENERIC\0");
            
            ImGui::Text("Servo PWM Range:");
            ImGui::SliderFloat("##servo_min", &camera_params.camera_servo_min, 1000.0f, 1500.0f);
            ImGui::SliderFloat("##servo_max", &camera_params.camera_servo_max, 1500.0f, 2000.0f);
            
            ImGui::Text("Gimbal Type:");
            ImGui::Combo("##gimbal_type", &camera_params.camera_gimbal_type,
                "NONE\0TWO_AXIS\0THREE_AXIS\0");
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Water")) {
            ImGui::Text("WATER SENSOR SETTINGS");
            ImGui::Separator();
            
            ImGui::Text("Pressure Offset:");
            ImGui::InputInt("##pressure_offset", &water_params.pressure_offset);
            
            ImGui::Text("Temperature Offset:");
            ImGui::InputFloat("##temp_offset", &water_params.temp_sensor_offset);
            
            ImGui::Text("Salinity Type:");
            ImGui::Combo("##salinity_type", &water_params.salinity_type,
                "FRESHWATER\0SALTWATER\0");
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Motor Config")) {
            ImGui::Text("FRAME AND MOTOR CONFIGURATION");
            ImGui::Separator();
            
            static int selected_frame = 0;
            ImGui::Text("Select Frame Type:");
            ImGui::Combo("##frame_type", &selected_frame,
                "Vectored (8x)\0Quadcopter (4x)\0Hexacopter (6x)\0Octocopter (8x)\0Custom\0");
            
            if (ImGui::Button("Apply Frame", ImVec2(150, 30))) {
                ui_log("Frame type changed");
            }
            
            ImGui::Separator();
            ImGui::Text("MOTOR REVERSAL AND CONFIGURATION");
            
            static bool motor_reversed[8] = {false};
            static float motor_angle[8] = {0, 45, 90, 135, 180, 225, 270, 315};
            
            ImGui::Columns(2, "motor_config", true);
            for (uint8_t i = 0; i < 8; i++) {
                char label[32];
                snprintf(label, sizeof(label), "M%d Reversed##rev%d", i+1, i);
                ImGui::Checkbox(label, &motor_reversed[i]);
                snprintf(label, sizeof(label), "M%d Angle##ang%d", i+1, i);
                ImGui::SliderFloat(label, &motor_angle[i], 0.0f, 360.0f, "%.1f°");
            }
            ImGui::Columns(1);
            
            if (ImGui::Button("Save Motor Config", ImVec2(200, 25))) {
                ui_log("Motor configuration saved");
            }
            
            ImGui::Separator();
            ImGui::Text("MOTOR TEST");
            static int test_motor = 0;
            ImGui::SliderInt("##test_motor_select", &test_motor, 0, 7);
            ImGui::SameLine();
            ImGui::Text("Motor: M%d", test_motor + 1);
            
            static float test_throttle = 0.0f;
            ImGui::SliderFloat("##test_throttle", &test_throttle, 0.0f, 1.0f, "%.2f");
            
            if (!g_connection.is_connected()) {
                ImGui::BeginDisabled();
            }
            
            if (ImGui::Button("SPIN", ImVec2(80, 25))) {
                // Send motor test command
                float motor_test_array[8] = {0};
                motor_test_array[test_motor] = test_throttle;
                g_control_sender.set_motor_test_mode(motor_test_array, true);
                
                char msg[64];
                snprintf(msg, sizeof(msg), "Spinning M%d at %.0f%%", test_motor+1, test_throttle*100);
                ui_log(msg);
            }
            ImGui::SameLine();
            if (ImGui::Button("STOP", ImVec2(80, 25))) {
                // Stop motor test
                float motor_test_array[8] = {0};
                g_control_sender.set_motor_test_mode(motor_test_array, false);
                ui_log("Motor stop");
            }
            
            if (!g_connection.is_connected()) {
                ImGui::EndDisabled();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Connect to Pixhawk first!");
            }
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Controller")) {
            ImGui::Text("CONTROLLER CONFIGURATION");
            ImGui::Separator();
            
            static int selected_profile = 0;
            ImGui::Text("Controller Profile:");
            ImGui::Combo("##profile", &selected_profile,
                "Default\0Racing\0Advanced\0");
            
            static float deadzone = 0.15f;
            static float expo = 0.2f;
            ImGui::Text("Deadzone:");
            ImGui::SliderFloat("##deadzone", &deadzone, 0.0f, 0.5f, "%.3f");
            ImGui::Text("Expo Curve:");
            ImGui::SliderFloat("##expo", &expo, 0.0f, 1.0f, "%.3f");
            
            ImGui::Separator();
            ImGui::Text("AXIS MAPPING");
            const char* axis_names[] = {"Left X", "Left Y", "Right X", "Right Y", "L Trigger", "R Trigger"};
            const char* output_names[] = {"Roll", "Pitch", "Yaw", "Throttle", "Unused"};
            
            static int axis_mapping[6] = {0, 1, 2, 3, 4, 5};
            for (int i = 0; i < 6; i++) {
                ImGui::Text("%s:", axis_names[i]);
                ImGui::SameLine();
                ImGui::Combo(("##axis_map_" + std::to_string(i)).c_str(), &axis_mapping[i], 
                    output_names, 5);
            }
            
            ImGui::Separator();
            ImGui::Text("BUTTON MAPPING");
            static int button_arm = 0;
            static int button_mode = 1;
            ImGui::Combo("ARM Button", &button_arm, "A\0B\0X\0Y\0LB\0RB\0Back\0Start\0");
            ImGui::Combo("MODE Button", &button_mode, "A\0B\0X\0Y\0LB\0RB\0Back\0Start\0");
            
            if (ImGui::Button("Save Controller Config", ImVec2(200, 25))) {
                ui_log("Controller configuration saved");
            }
            ImGui::SameLine();
            if (ImGui::Button("Calibrate", ImVec2(100, 25))) {
                ui_log("Controller calibration started");
            }
            
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}

void ui_render()
{
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), g_renderer);
}

void ui_log(const char *message)
{
    if (!message) return;
    
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    
    std::string log_entry = std::string("[") + timestamp + "] " + message;
    g_log_messages.push_back(log_entry);
    
    if (g_log_messages.size() > MAX_LOG_MESSAGES) {
        g_log_messages.erase(g_log_messages.begin());
    }
}

void ui_shutdown(void)
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void ui_send_control_packet(const ControllerState &ctrl)
{
    if (!g_connection.is_connected()) return;
    
    // Update control sender with current controller state
    g_control_sender.set_control_mode(ctrl);
    g_control_sender.set_armed(g_armed);
    
    // Serialize and send the packet
    auto packet_data = g_control_sender.serialize();
    if (!packet_data.empty()) {
        g_connection.send(packet_data.data(), packet_data.size());
    }
}

void ui_receive_telemetry()
{
    uint8_t buffer[2048];
    uint16_t received_len = 0;
    
    if (!g_connection.is_connected()) return;
    
    // Try to receive data
    if (g_connection.receive(buffer, sizeof(buffer), received_len) && received_len > 0) {
        TelemetryPacket packet;
        if (g_telemetry_parser.parse_packet(buffer, received_len, packet)) {
            // Update telemetry data from packet
            telemetry_data.battery_voltage = packet.state.battery.voltage;
            telemetry_data.battery_current = packet.state.battery.current;
            telemetry_data.battery_percentage = packet.state.battery.percentage;
            
            telemetry_data.gyro_x = packet.state.sensors.gyro_x;
            telemetry_data.gyro_y = packet.state.sensors.gyro_y;
            telemetry_data.gyro_z = packet.state.sensors.gyro_z;
            
            telemetry_data.accel_x = packet.state.sensors.accel_x;
            telemetry_data.accel_y = packet.state.sensors.accel_y;
            telemetry_data.accel_z = packet.state.sensors.accel_z;
            
            telemetry_data.mag_x = packet.state.sensors.mag_x;
            telemetry_data.mag_y = packet.state.sensors.mag_y;
            telemetry_data.mag_z = packet.state.sensors.mag_z;
            
            telemetry_data.depth = packet.state.sensors.depth;
            telemetry_data.temperature = packet.state.sensors.temperature;
            telemetry_data.pressure = packet.state.sensors.pressure;
            
            telemetry_data.roll = packet.state.roll;
            telemetry_data.pitch = packet.state.pitch;
            telemetry_data.yaw = packet.state.yaw;
            
            telemetry_data.armed = packet.state.armed;
            telemetry_data.flight_mode = packet.state.flight_mode;
        }
    }
}
