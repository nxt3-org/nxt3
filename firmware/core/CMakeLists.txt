cmake_minimum_required(VERSION 3.10)

project(NXTcore VERSION 1.32.1
        DESCRIPTION "NXT firmware core"
        LANGUAGES C)

add_executable(nxt3.elf
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_6.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_4.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Running.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/cCmdWriteIOMapOffsetsFile.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_8.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Devices.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Wait.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Font.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_14.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_12.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Status.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_2.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_5.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Ui.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu04.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu05.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu07.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_9.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Display.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_7.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Icons.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_16.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_1.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Test2.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/LowBattery.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Step.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Fail.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_10.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Ok.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu02.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Port.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Info.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu06.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu03.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Mainmenu.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu01.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Test1.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Cursor.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_3.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_15.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Connections.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_11.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/RCXintro_13.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_pnp.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_output.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_ui.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_cmd_bytecodes.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_comm.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_display.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/m_sched.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_ioctrl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_button.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/modules.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_cmd.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_sound.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_output_reg.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_sound.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/stdconst.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_cmd.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/d_bt.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_lowspeed.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_lowspeed.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_loader.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_ui.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_sound_adpcm.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_input.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_pnp.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_comm.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_ioctrl.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_input.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_button.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_output.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_loader.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/c_display.iom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_pnp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_ui.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Functions.inl
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_comm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_lowspeed.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/d_bt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_output.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_display.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_output_reg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BtTest.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/m_sched.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_sound.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_cmd.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_ioctrl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_input.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_cmd_drawing.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_sound_adpcm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_loader.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_button.c
        )

target_include_directories(nxt3.elf PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/generated
        ${CMAKE_CURRENT_BINARY_DIR}/generated
        )
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
