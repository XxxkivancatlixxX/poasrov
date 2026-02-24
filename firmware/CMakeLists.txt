cmake_minimum_required(VERSION 3.16)
project(PixhawkFirmware)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(STM32_FAMILY STM32F4xx)
set(STM32_CHIP STM32F427VI)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
