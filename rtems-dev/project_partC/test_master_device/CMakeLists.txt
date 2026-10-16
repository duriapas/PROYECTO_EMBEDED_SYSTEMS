cmake_minimum_required(VERSION 3.22)
project(test_master_device C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

message(STATUS "detected Architecture: ${CMAKE_SYSTEM_PROCESSOR}-")

# select object file upon the architecture
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    message(STATUS "Compiling for x86_64")
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/test_master_device_x86-64.o")
        add_library(test_master_device_obj OBJECT test_master_device_x86-64.c)
        add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/test_master_device_x86-64.o
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_OBJECTS:test_master_device_obj>
            ${CMAKE_SOURCE_DIR}/test_master_device_x86-64.o
    )
    else()
        message(STATUS "Skipping building test_master_device_x86-64.o")
    endif()
    add_executable(test_master_device ${CMAKE_SOURCE_DIR}/test_master_device_x86-64.o)
    set_target_properties(test_master_device PROPERTIES LINKER_LANGUAGE C)

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    message(STATUS "Compiling for ARM64")
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/test_master_device_AppleM1.o")
        add_library(test_master_device_obj OBJECT test_master_device_AppleM1.c)
        add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/test_master_device_AppleM1.o
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_OBJECTS:test_master_device_obj>
            ${CMAKE_SOURCE_DIR}/test_master_device_AppleM1.o
    )
    else()
        message(STATUS "Skipping building test_master_device_AppleM1.o")
    endif()
    add_executable(test_master_device ${CMAKE_SOURCE_DIR}/test_master_device_AppleM1.o)
    set_target_properties(test_master_device PROPERTIES LINKER_LANGUAGE C)

else()
    message(WARNING "Unknown Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()