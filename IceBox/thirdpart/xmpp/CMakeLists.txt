LIST (APPEND INCLUDES 
    ${CMAKE_CURRENT_LIST_DIR}/include
    )

LIST (APPEND LIBS_DIRECTORIES
    ${CMAKE_CURRENT_LIST_DIR}/lib
    )

LIST (APPEND LIBRARIES 
    xmpp
    )    

###TODO for debug use
IF (EXISTS ${CMAKE_CURRENT_LIST_DIR}/lib/libxmpp.so) 
    EXECUTE_PROCESS(
        COMMAND cp ${CMAKE_CURRENT_LIST_DIR}/lib/libxmpp.so  ${EXTERNAL_LIBS_PATH}/
        )
ENDIF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/lib/libxmpp.so)
