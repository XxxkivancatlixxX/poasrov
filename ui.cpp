#include "ui.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <vector>
#include <string>
#include <ctime>

static SDL_Window   *g_window   = NULL;
static SDL_Renderer *g_renderer = NULL;

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
            
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
            if (!g_armed) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
                if (ImGui::Button("ARM SYSTEM", ImVec2(-1, 40))) {
                    g_armed = true;
                }
                ImGui::PopStyleColor(2);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                if (ImGui::Button("DISARM SYSTEM", ImVec2(-1, 40))) {
                    g_armed = false;
                }
                ImGui::PopStyleColor(2);
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
            ImGui::Text("BATTERY SETTINGS");
            ImGui::Separator();
            
            ImGui::Text("Battery Voltage:");
            ImGui::ProgressBar(battery_params.battery_voltage / 16.0f, ImVec2(-1, 20));
            ImGui::Text("%.2f V", battery_params.battery_voltage);
            
            ImGui::Text("Battery Capacity:");
            ImGui::InputFloat("mAh##capacity", &battery_params.battery_capacity);
            
            ImGui::Text("Battery Current:");
            ImGui::ProgressBar(battery_params.battery_current / 50.0f, ImVec2(-1, 20));
            ImGui::Text("%.2f A", battery_params.battery_current);
            
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
