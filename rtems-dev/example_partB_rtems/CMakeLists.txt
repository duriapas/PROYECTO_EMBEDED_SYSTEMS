cmake_minimum_required(VERSION 3.22)
project(example C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

# Ejecutable principal
add_executable(example example_rtems.c)
