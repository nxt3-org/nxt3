function(add_bitmap TARGET SRC DST)
    target_sources("${TARGET}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}")
    add_custom_command(
            COMMENT "Generating bitmap ${DST}"
            DEPENDS "${CMAKE_SOURCE_DIR}/resources/${SRC}"
            "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mkbitmap.py" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/utils/imagepipe.py"
            OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mkbitmap.py"
            -i "${CMAKE_SOURCE_DIR}/resources/${SRC}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
    )
endfunction()
