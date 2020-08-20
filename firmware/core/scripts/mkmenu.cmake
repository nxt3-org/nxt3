function(add_menu_common TARGET COMMONJSON DST)
    target_sources("${TARGET}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}")
    add_custom_command(
            COMMENT "Generating common menu defs ${DST}"
            DEPENDS "${CMAKE_SOURCE_DIR}/resources/${COMMONJSON}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mkmenu.py"
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mkmenu.py"
            -e "${CMAKE_SOURCE_DIR}/resources/${COMMONJSON}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
    )
endfunction()

function(add_menu TARGET COMMONJSON ICONJSON MENUJSON DST)
    target_sources("${TARGET}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}")
    add_custom_command(
            COMMENT "Generating menu ${DST}"
            DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mkmenu.py"
            "${CMAKE_SOURCE_DIR}/resources/${COMMONJSON}"
            "${CMAKE_SOURCE_DIR}/resources/${ICONJSON}"
            "${CMAKE_SOURCE_DIR}/resources/${MENUJSON}"
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/mkmenu.py"
            -i "${CMAKE_SOURCE_DIR}/resources/${MENUJSON}"
            -e "${CMAKE_SOURCE_DIR}/resources/${COMMONJSON}"
            -m "${CMAKE_SOURCE_DIR}/resources/${ICONJSON}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/generated/${DST}"
    )
endfunction()
