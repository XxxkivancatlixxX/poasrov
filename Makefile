# Makefile for ROV GUI (SDL2 + FFmpeg + Dear ImGui)

CXX      := g++
CXXFLAGS := -Wall -Wextra -O2 -g

# Qt Quick (Qt5) flags – requires Qt5Quick/Qt5Qml/Qt5Gui/Qt5Core dev packages
QT_CFLAGS := $(shell pkg-config --cflags Qt5Quick Qt5Qml Qt5Gui Qt5Core)
QT_LIBS   := $(shell pkg-config --libs Qt5Quick Qt5Qml Qt5Gui Qt5Core)

# SDL2 for joystick support
SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS   := $(shell pkg-config --libs sdl2)

# FFmpeg for video streaming
FFMPEG_CFLAGS := $(shell pkg-config --cflags libavformat libavcodec libavutil libswscale)
FFMPEG_LIBS   := $(shell pkg-config --libs libavformat libavcodec libavutil libswscale)

MOC      := moc

MAVLINK_DIR := libs/c_library_v2

INCLUDES := -I. -I$(MAVLINK_DIR)/common

SRCS := \
    main_qt.cpp \
    Backend.cpp \
    control_sender.cpp \
    tcp_client.cpp \
    connection.cpp \
    telemetry_parser.cpp \
    mavlink_parser.cpp \
    protocol_handler.cpp \
    ROV.cpp \
    input.cpp \
    joystick_control.cpp \
    video_provider.cpp \
    camera_image_provider.cpp

MOC_SRCS := moc_Backend.cpp moc_VideoProvider.cpp

OBJS := $(SRCS:.cpp=.o) $(MOC_SRCS:.cpp=.o)

TARGET := rov_gui

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(QT_LIBS) $(SDL_LIBS) $(FFMPEG_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(QT_CFLAGS) $(SDL_CFLAGS) $(FFMPEG_CFLAGS) -c $< -o $@

moc_Backend.cpp: Backend.h
	$(MOC) $(QT_CFLAGS) $< -o $@

moc_VideoProvider.cpp: video_provider.h
	$(MOC) $(QT_CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) moc_Backend.cpp moc_VideoProvider.cpp

.PHONY: all clean

# Test joystick standalone
test_joystick: test_joystick.cpp input.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -o $@ $^ $(SDL_LIBS)

.PHONY: test
test: test_joystick
	./test_joystick
