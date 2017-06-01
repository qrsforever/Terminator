LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/AppSetting.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Setting.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SettingListener.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SettingModule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SettingModuleApp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SettingModuleNetwork.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SettingModuleWeather.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SysSetting.cpp
    )
