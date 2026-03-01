cmake_minimum_required(VERSION 3.16)
project(rov_qt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Tell CMake where Homebrew's Qt lives (adjust if your brew prefix is different)
# You can find it with: brew --prefix qt@6
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@6" "/usr/local/opt/qt@6" ${CMAKE_PREFIX_PATH})

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# SDL2 via pkg-config (brew supplies .pc)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

# FFmpeg libs (brew: libavformat.dylib etc.)
find_library(AVFORMAT_LIB avformat REQUIRED)
find_library(AVCODEC_LIB  avcodec  REQUIRED)
find_library(AVUTIL_LIB   avutil   REQUIRED)
find_library(SWSCALE_LIB  swscale  REQUIRED)

add_executable(rov_qt
    # Qt GUI
    main.cpp
    MainWindow.h MainWindow.cpp
    ConnectionTab.h ConnectionTab.cpp
    VideoWidget.h VideoWidget.cpp
    VideoDecoder.h VideoDecoder.cpp

    # Your reused logic
    input.h input.cpp
    connection.h connection.cpp
    control_sender.h control_sender.cpp
    telemetry_parser.h telemetry_parser.cpp
    telemetry_receiver.h telemetry_receiver.cpp
)

target_include_directories(rov_qt PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(rov_qt
    PRIVATE
    Qt6::Widgets
    ${SDL2_LIBRARIES}
    ${AVFORMAT_LIB}
    ${AVCODEC_LIB}
    ${AVUTIL_LIB}
    ${SWSCALE_LIB}
)

# On macOS, bundle Qt plugins automatically if you later use `macdeployqt`
set_target_properties(rov_qt PROPERTIES
    MACOSX_BUNDLE TRUE
)
