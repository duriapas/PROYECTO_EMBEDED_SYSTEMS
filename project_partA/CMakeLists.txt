# 1. Sets the minimum required CMake version.
cmake_minimum_required(VERSION 3.30)

# 2. Defines the first project and enables both C and C++ compilers.
project(MiProyecto C CXX)

# 3. Sets standard versions for C and C++ globally.
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 4. Locate GTest a pthreads
find_package(GTest REQUIRED)
find_package (Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# 5. Compile and link temperature sensor test
add_executable(test_temperature test_temperature.cpp slave_device.c)
target_link_libraries(test_temperature ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# 6. Compile and link position sensor test
add_executable(test_position test_position.cpp slave_device.c)
target_link_libraries(test_position ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# 7. Compile and link control temperature test
add_executable(test_control_temp test_control_temp.cpp master_device.c)
target_link_libraries(test_control_temp ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# 8. Compile and link comuniaction protocol test
add_executable(test_protocol test_protocol.cpp master_device.c slave_device.c)
target_link_libraries(test_protocol ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
