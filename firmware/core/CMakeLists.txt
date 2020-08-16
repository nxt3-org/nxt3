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
    ${CMAKE_CURRENT_SOURCE_DIR}/c_output_reg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_sound.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_sound_adpcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_ui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/c_pnp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/d_bt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/m_sched.c
        )

target_include_directories(nxt3.elf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nxt3.elf nxthal nxthal_lms2012 -lm -lrt)

set_target_properties(nxt3.elf PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
)

add_custom_command(TARGET nxt3.elf
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} --only-keep-debug nxt3.elf nxt3.dbg
        COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=nxt3.dbg nxt3.elf
        COMMAND ${CMAKE_STRIP} --strip-all nxt3.elf
        )
