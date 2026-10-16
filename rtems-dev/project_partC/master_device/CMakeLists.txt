cmake_minimum_required(VERSION 3.22)
project(master_device C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

# Ejecutable principal
add_executable(master_device master_device.c main.c)
