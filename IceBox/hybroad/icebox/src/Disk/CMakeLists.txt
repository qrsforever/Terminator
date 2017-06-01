LIST (APPEND INCLUDES ${CMAKE_CURRENT_LIST_DIR})

# INCLUDE (${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt)

LIST (APPEND CLOUD_CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/DiskDevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DiskDeviceManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Filesystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Ext4.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Fat32.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Ntfs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Partition.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DiskAssertions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DiskCopyQueue.cpp
    ${CMAKE_CURRENT_LIST_DIR}/depend/partition.c
    ${CMAKE_CURRENT_LIST_DIR}/depend/i386_sys_types.c
    )
