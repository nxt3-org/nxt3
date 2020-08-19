cmake_minimum_required(VERSION 3.10)

project(NXTcore VERSION 1.32.1
        DESCRIPTION "NXT firmware core"
        LANGUAGES C)

include(scripts/mktexts.cmake)
include(scripts/mkbitmap.cmake)
include(scripts/mkfont.cmake)
include(scripts/mkmenu.cmake)
include(scripts/mkicons.cmake)

add_executable(nxt3.elf
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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_cmd_iomap_writer.c
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
        ${CMAKE_CURRENT_BINARY_DIR}/generated
        ${CMAKE_CURRENT_SOURCE_DIR}/generated
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

add_texts( nxt3.elf strings/english.json      Ui.h)
add_font(  nxt3.elf font/normal.json          Font.h)
add_icons( nxt3.elf font/port.json            Port.h)
add_icons( nxt3.elf status/status.json        Status.h)
add_icons( nxt3.elf menu/icons/icons.json     Icons.h)
add_icons( nxt3.elf running/animation.json    Running.h)
add_icons( nxt3.elf comm/devices.json         Devices.h)
add_icons( nxt3.elf obp/steps/steps.json      Step.h)
add_icons( nxt3.elf comm/empty_slots.json     Connections.h)
add_bitmap(nxt3.elf bitmaps/Test1.json        Test1.h)
add_bitmap(nxt3.elf bitmaps/Test2.json        Test2.h)
add_bitmap(nxt3.elf bitmaps/LowBattery.json   LowBattery.h)
add_bitmap(nxt3.elf bitmaps/Cursor.json       Cursor.h)
add_bitmap(nxt3.elf bitmaps/Wait.json         Wait.h)
add_bitmap(nxt3.elf bitmaps/Fail.json         Fail.h)
add_bitmap(nxt3.elf bitmaps/Info.json         Info.h)
add_bitmap(nxt3.elf bitmaps/Ok.json           Ok.h)
add_bitmap(nxt3.elf bitmaps/Display.json      Display.h)
add_bitmap(nxt3.elf intro/frame1.json         RCXintro_1.h)
add_bitmap(nxt3.elf intro/frame2.json         RCXintro_2.h)
add_bitmap(nxt3.elf intro/frame3.json         RCXintro_3.h)
add_bitmap(nxt3.elf intro/frame4.json         RCXintro_4.h)
add_bitmap(nxt3.elf intro/frame5.json         RCXintro_5.h)
add_bitmap(nxt3.elf intro/frame6.json         RCXintro_6.h)
add_bitmap(nxt3.elf intro/frame7.json         RCXintro_7.h)
add_bitmap(nxt3.elf intro/frame8.json         RCXintro_8.h)
add_bitmap(nxt3.elf intro/frame9.json         RCXintro_9.h)
add_bitmap(nxt3.elf intro/frame10.json        RCXintro_10.h)
add_bitmap(nxt3.elf intro/frame11.json        RCXintro_11.h)
add_bitmap(nxt3.elf intro/frame12.json        RCXintro_12.h)
add_bitmap(nxt3.elf intro/frame13.json        RCXintro_13.h)
add_bitmap(nxt3.elf intro/frame14.json        RCXintro_14.h)
add_bitmap(nxt3.elf intro/frame15.json        RCXintro_15.h)
add_bitmap(nxt3.elf intro/frame16.json        RCXintro_16.h)
#add_menu(  nxt3.elf MENUES/Mainmenu.json      Mainmenu.h)
#add_menu(  nxt3.elf MENUES/Submenu01.json     Submenu01.h)
#add_menu(  nxt3.elf MENUES/Submenu02.json     Submenu02.h)
#add_menu(  nxt3.elf MENUES/Submenu03.json     Submenu03.h)
#add_menu(  nxt3.elf MENUES/Submenu04.json     Submenu04.h)
#add_menu(  nxt3.elf MENUES/Submenu05.json     Submenu05.h)
#add_menu(  nxt3.elf MENUES/Submenu06.json     Submenu06.h)
#add_menu(  nxt3.elf MENUES/Submenu07.json     Submenu07.h)
