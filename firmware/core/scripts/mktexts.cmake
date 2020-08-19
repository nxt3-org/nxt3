function(add_texts TARGET SRC DST)
    target_sources("${TARGET}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}")
    add_custom_command(
            COMMENT "Generating string file ${DST}"
            DEPENDS "${CMAKE_SOURCE_DIR}/resources/${SRC}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mktexts.py"
            OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mktexts.py"
            -i "${CMAKE_SOURCE_DIR}/resources/${SRC}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
    )
endfunction()
