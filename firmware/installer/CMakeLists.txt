cmake_minimum_required(VERSION 3.10)

project(NXTInstaller
        VERSION 0.1
        DESCRIPTION "NXT firmware installer"
        LANGUAGES)

add_custom_target(installer
        DEPENDS installer.sh installer.rbf installer.uf2
        )

add_custom_command(
        OUTPUT installer.sh installer.rbf installer.uf2
        COMMENT "Generating NXT3 SFX installer"
        DEPENDS nxt3.elf battd.elf
        ${CMAKE_CURRENT_SOURCE_DIR}/mksfx.py
        ${CMAKE_CURRENT_SOURCE_DIR}/sfx.sh
        COMMAND /usr/bin/env python3 "${CMAKE_CURRENT_SOURCE_DIR}/mksfx.py"
        --output-sh "${CMAKE_CURRENT_BINARY_DIR}/NXT3_Installer.elf"
        --output-rbf "${CMAKE_CURRENT_BINARY_DIR}/NXT3_Installer.rbf"
        --output-uf2 "${CMAKE_CURRENT_BINARY_DIR}/NXT3_Installer.uf2"
        --nxt3 "$<TARGET_FILE:nxt3.elf>"
        --battd "$<TARGET_FILE:battd.elf>"
        --sfx "${CMAKE_CURRENT_SOURCE_DIR}/sfx.sh"
        --include-data "${CMAKE_SOURCE_DIR}/resources/sounds/! Attention.rso"
        --include-data "${CMAKE_SOURCE_DIR}/resources/sounds/! Startup.rso"
        --include-data "${CMAKE_SOURCE_DIR}/resources/sounds/! Click.rso"
        --include-data "${CMAKE_SOURCE_DIR}/resources/obp/RPGReader.sys"
        --include-data "${CMAKE_SOURCE_DIR}/resources/tryme/faceclosed.ric"
        --include-data "${CMAKE_SOURCE_DIR}/resources/tryme/faceopen.ric"
        --include-data "${CMAKE_SOURCE_DIR}/resources/tryme/Woops.rso"
        --include-data "${CMAKE_SOURCE_DIR}/resources/tryme/Try-Light.rtm"
        --include-data "${CMAKE_SOURCE_DIR}/resources/tryme/Try-Motor.rtm"
        --include-data "${CMAKE_SOURCE_DIR}/resources/tryme/Try-Sound.rtm"
        --include-data "${CMAKE_SOURCE_DIR}/resources/tryme/Try-Touch.rtm"
        --include-data "${CMAKE_SOURCE_DIR}/resources/tryme/Try-Ultrasonic.rtm"
)
