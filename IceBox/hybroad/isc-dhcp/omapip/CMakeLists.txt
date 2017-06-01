INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_LIST_DIR}/
    )

LIST (APPEND DHCLIENT_SRC_LIST 
    ${CMAKE_CURRENT_LIST_DIR}/alloc.c
    ${CMAKE_CURRENT_LIST_DIR}/auth.c
    ${CMAKE_CURRENT_LIST_DIR}/connection.c
    ${CMAKE_CURRENT_LIST_DIR}/dispatch.c
    ${CMAKE_CURRENT_LIST_DIR}/generic.c
    ${CMAKE_CURRENT_LIST_DIR}/hash.c
    ${CMAKE_CURRENT_LIST_DIR}/message.c
    ${CMAKE_CURRENT_LIST_DIR}/protocol.c
    ${CMAKE_CURRENT_LIST_DIR}/support.c
    ${CMAKE_CURRENT_LIST_DIR}/toisc.c
    ${CMAKE_CURRENT_LIST_DIR}/array.c
    ${CMAKE_CURRENT_LIST_DIR}/buffer.c
    ${CMAKE_CURRENT_LIST_DIR}/convert.c
    ${CMAKE_CURRENT_LIST_DIR}/errwarn.c
    ${CMAKE_CURRENT_LIST_DIR}/handle.c
    ${CMAKE_CURRENT_LIST_DIR}/listener.c
    ${CMAKE_CURRENT_LIST_DIR}/mrtrace.c
    ${CMAKE_CURRENT_LIST_DIR}/result.c
    ${CMAKE_CURRENT_LIST_DIR}/trace.c
    )
