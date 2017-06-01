INCLUDE_DIRECTORIES (
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/depend
    )

LIST (APPEND CLOUD_C_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/depend/cooee.c 
    ${CMAKE_CURRENT_LIST_DIR}/depend/easy_setup.c 
    ${CMAKE_CURRENT_LIST_DIR}/depend/easy_setup_linux.c 
    ${CMAKE_CURRENT_LIST_DIR}/depend/scan.c
    ${CMAKE_CURRENT_LIST_DIR}/depend/akiss.c
    ${CMAKE_CURRENT_LIST_DIR}/depend/changhong.c
    ${CMAKE_CURRENT_LIST_DIR}/depend/neeze.c 
    )

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/EasySetup.cpp
    )
