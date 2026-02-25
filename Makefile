# Makefile for ROV GUI (SDL2 + FFmpeg + Dear ImGui)

CXX      := g++
CXXFLAGS := -Wall -Wextra -O2 -g

SDL2_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL2_LIBS   := $(shell pkg-config --libs sdl2)

FFMPEG_LIBS := -lavformat -lavcodec -lavutil -lswscale

IMGUI_DIR := imgui/imgui

INCLUDES := -I. -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends

SRCS := \
    main.cpp \
    ui.cpp \
    input.cpp \
    video.cpp \
    control_sender.cpp \
    tcp_client.cpp \
    connection.cpp \
    telemetry_parser.cpp \
    $(IMGUI_DIR)/imgui.cpp \
    $(IMGUI_DIR)/imgui_draw.cpp \
    $(IMGUI_DIR)/imgui_widgets.cpp \
    $(IMGUI_DIR)/imgui_tables.cpp \
    $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp \
    $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp

OBJS := $(SRCS:.cpp=.o)

TARGET := rov_gui

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL2_LIBS) $(FFMPEG_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SDL2_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
