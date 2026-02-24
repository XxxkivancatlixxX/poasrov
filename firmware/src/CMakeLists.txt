add_executable(firmware
    main.cpp
    mavlink_handler.cpp
    ethernet_comm.cpp
    pixhawk_control.cpp
    mission_control.cpp
)

target_include_directories(firmware PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
