cmake_minimum_required(VERSION 3.10)

project(NXTcore VERSION 1.32.1
        DESCRIPTION "NXT firmware core"
        LANGUAGES C)

add_executable(nxt3.elf
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Running.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/cCmdWriteIOMapOffsetsFile.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Devices.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Font.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Status.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu04.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu05.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu07.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Icons.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Step.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu02.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Port.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu06.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu03.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Mainmenu.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Submenu01.rms
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Connections.txt
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Ui.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Test1.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Test2.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/LowBattery.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Fail.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Ok.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Wait.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Info.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Display.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/Cursor.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_1.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_2.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_3.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_4.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_5.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_6.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_7.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_8.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_9.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_10.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_11.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_12.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_13.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_14.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_15.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/RCXintro_16.h
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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Functions.inl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_comm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_lowspeed.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/d_bt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_output.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_display.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_output_reg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BtTest.inc.h
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

add_custom_command(
        COMMENT "Generating resources"
        OUTPUT
        generated/Test1.h
        generated/Test2.h
        generated/LowBattery.h
        generated/Cursor.h
        generated/Wait.h
        generated/Fail.h
        generated/Info.h
        generated/Ok.h
        generated/Display.h
        generated/RCXintro_1.h
        generated/RCXintro_2.h
        generated/RCXintro_3.h
        generated/RCXintro_4.h
        generated/RCXintro_5.h
        generated/RCXintro_6.h
        generated/RCXintro_7.h
        generated/RCXintro_8.h
        generated/RCXintro_9.h
        generated/RCXintro_10.h
        generated/RCXintro_11.h
        generated/RCXintro_12.h
        generated/RCXintro_13.h
        generated/RCXintro_14.h
        generated/RCXintro_15.h
        generated/RCXintro_16.h
        generated/Ui.h
        DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/generate_resources.py
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Test1.bmp
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Test2.bmp
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/LowBattery.bmp
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Cursor.bmp
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Wait.bmp
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Fail.bmp
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Info.bmp
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Ok.bmp
        ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Display.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_1.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_2.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_3.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_4.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_5.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_6.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_7.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_8.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_9.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_10.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_11.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_12.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_13.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_14.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_15.bmp
        ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_16.bmp
        ${CMAKE_SOURCE_DIR}/resources/TEXT/strings.json
        COMMAND
        /usr/bin/env python3 "${CMAKE_CURRENT_SOURCE_DIR}/generate_resources.py"
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Test1.bmp      generated/Test1.h         0  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Test2.bmp      generated/Test2.h         0  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/LowBattery.bmp generated/LowBattery.h    2  8
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Cursor.bmp     generated/Cursor.h        0  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Wait.bmp       generated/Wait.h          0  8
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Fail.bmp       generated/Fail.h          0  8
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Info.bmp       generated/Info.h          0  8
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Ok.bmp         generated/Ok.h           42 48
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Display.bmp    generated/Display.h      14 16
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_1.bmp   generated/RCXintro_1.h   16  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_2.bmp   generated/RCXintro_2.h   16  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_3.bmp   generated/RCXintro_3.h   16  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_4.bmp   generated/RCXintro_4.h   16  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_5.bmp   generated/RCXintro_5.h   23  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_6.bmp   generated/RCXintro_6.h   28  0
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_7.bmp   generated/RCXintro_7.h   35 16
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_8.bmp   generated/RCXintro_8.h   44 24
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_9.bmp   generated/RCXintro_9.h   52 24
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_10.bmp  generated/RCXintro_10.h  56 32
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_11.bmp  generated/RCXintro_11.h  58 32
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_12.bmp  generated/RCXintro_12.h   3 32
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_13.bmp  generated/RCXintro_13.h   3 32
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_14.bmp  generated/RCXintro_14.h   3 32
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_15.bmp  generated/RCXintro_15.h   3 32
        bitmap ${CMAKE_SOURCE_DIR}/resources/INTRO/RCXintro_16.bmp  generated/RCXintro_16.h   3 32
        bitmap ${CMAKE_SOURCE_DIR}/resources/BITMAPS/Test1.bmp      generated/Test1.h         0  0
        texts  ${CMAKE_SOURCE_DIR}/resources/TEXT/strings.json      generated/Ui.h
)
