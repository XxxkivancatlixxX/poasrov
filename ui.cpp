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
static float g_motor_test[8] = {0};
static std::vector<std::string> g_log_messages;
static const int MAX_LOG_MESSAGES = 100;

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
    ImGui::Text("ROV Control System");
    ImGui::SameLine(ImGui::GetWindowWidth() - 250);
    
    if (ctrl.connected) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Controller: CONNECTED");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Controller: DISCONNECTED");
    }
    ImGui::Separator();
    
    float available_height = ImGui::GetContentRegionAvail().y;
    float available_width = ImGui::GetContentRegionAvail().x;
    
    ImGui::BeginChild("VideoPanel", ImVec2(available_width * 0.72f, available_height), false, ImGuiWindowFlags_NoScrollbar);
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
    
    ImGui::BeginChild("ControlPanel", ImVec2(available_width * 0.27f, available_height), false);
    
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
        
        ImGui::Separator();
        ImGui::Text("BUTTONS");
        ImGui::Text("A: %s B: %s", ctrl.button_a ? "PRESSED" : "---", ctrl.button_b ? "PRESSED" : "---");
        ImGui::Text("X: %s Y: %s", ctrl.button_x ? "PRESSED" : "---", ctrl.button_y ? "PRESSED" : "---");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Not Connected");
    }
    
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
    
    ImGui::BeginChild("LogWindow", ImVec2(0, 0), true, ImGuiWindowFlags_NoMove);
    for (const auto& msg : g_log_messages) {
        ImGui::TextWrapped("%s", msg.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    
    ImGui::EndChild();
    ImGui::End();
}

void ui_render()
{
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), g_renderer);
}

void ui_shutdown(void)
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
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
