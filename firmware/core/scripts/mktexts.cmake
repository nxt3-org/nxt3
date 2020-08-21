function(add_texts TARGET SRC DST)
    target_sources("${TARGET}" PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}.c"
            "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}.h"
            )
    add_custom_command(
            COMMENT "Generating string file ${DST}"
            DEPENDS
            "${CMAKE_SOURCE_DIR}/resources/${SRC}"
            "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mktexts.py"
            OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}.c"
            "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}.h"
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mktexts.py"
            --in "${CMAKE_SOURCE_DIR}/resources/${SRC}"
            --src "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}.c"
            --hdr "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}.h"
    )
endfunction()
