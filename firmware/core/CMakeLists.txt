cmake_minimum_required(VERSION 3.10)

project(NXTcore  VERSION 1.32.1
                 DESCRIPTION "NXT firmware core"
                 LANGUAGES C)

add_executable(nxt3.elf
    ${CMAKE_CURRENT_SOURCE_DIR}/c_button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_cmd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_comm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_display.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_input.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_ioctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_loader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_lowspeed.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_output.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_sound.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_ui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_bt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_button_ev3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_display_ev3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_hispeed.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_input.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_ioctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_loader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_loader_ev3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_lowspeed.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_output.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_output_ev3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_sound_adpcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_sound.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_timer_ev3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/m_sched.c
)

target_include_directories(nxt3.elf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(nxt3.elf PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
)
