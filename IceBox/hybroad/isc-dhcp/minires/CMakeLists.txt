
INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_LIST_DIR}
    )

LIST (APPEND DHCLIENT_SRC_LIST 
    ${CMAKE_CURRENT_LIST_DIR}/ns_parse.c
    ${CMAKE_CURRENT_LIST_DIR}/ns_sign.c
    ${CMAKE_CURRENT_LIST_DIR}/res_comp.c
    ${CMAKE_CURRENT_LIST_DIR}/res_init.c
    ${CMAKE_CURRENT_LIST_DIR}/res_mkupdate.c
    ${CMAKE_CURRENT_LIST_DIR}/res_send.c
    ${CMAKE_CURRENT_LIST_DIR}/res_update.c
    ${CMAKE_CURRENT_LIST_DIR}/ns_name.c
    ${CMAKE_CURRENT_LIST_DIR}/ns_samedomain.c
    ${CMAKE_CURRENT_LIST_DIR}/ns_verify.c
    ${CMAKE_CURRENT_LIST_DIR}/res_findzonecut.c
    ${CMAKE_CURRENT_LIST_DIR}/res_mkquery.c
    ${CMAKE_CURRENT_LIST_DIR}/res_sendsigned.c
    )
