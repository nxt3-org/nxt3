function(add_font TARGET SRC DST)
    target_sources("${TARGET}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}")
    add_custom_command(
            COMMENT "Generating font ${DST}"
            DEPENDS "${CMAKE_SOURCE_DIR}/resources/${SRC}"
            "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mkfont.py" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/utils/imagepipe.py"
            OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mkfont.py"
            -i "${CMAKE_SOURCE_DIR}/resources/${SRC}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
    )
endfunction()
