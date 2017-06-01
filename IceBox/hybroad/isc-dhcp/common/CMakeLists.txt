INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_LIST_DIR}
    )

LIST (APPEND DHCLIENT_SRC_LIST 
    ${CMAKE_CURRENT_LIST_DIR}/alloc.c
    ${CMAKE_CURRENT_LIST_DIR}/comapi.c
    ${CMAKE_CURRENT_LIST_DIR}/ctrace.c
    ${CMAKE_CURRENT_LIST_DIR}/dispatch.c
    ${CMAKE_CURRENT_LIST_DIR}/dns.c
    ${CMAKE_CURRENT_LIST_DIR}/execute.c
    ${CMAKE_CURRENT_LIST_DIR}/inet.c
    ${CMAKE_CURRENT_LIST_DIR}/memory.c
    ${CMAKE_CURRENT_LIST_DIR}/options.c
    ${CMAKE_CURRENT_LIST_DIR}/parse.c
    ${CMAKE_CURRENT_LIST_DIR}/socket.c
    ${CMAKE_CURRENT_LIST_DIR}/bpf.c
    ${CMAKE_CURRENT_LIST_DIR}/conflex.c
    ${CMAKE_CURRENT_LIST_DIR}/discover.c
    ${CMAKE_CURRENT_LIST_DIR}/ethernet.c
    ${CMAKE_CURRENT_LIST_DIR}/lpf.c
    ${CMAKE_CURRENT_LIST_DIR}/packet.c
    ${CMAKE_CURRENT_LIST_DIR}/print.c
    ${CMAKE_CURRENT_LIST_DIR}/tables.c
    ${CMAKE_CURRENT_LIST_DIR}/tree.c
    ${CMAKE_CURRENT_LIST_DIR}/log.c
    )
