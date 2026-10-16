# toolchain-rtems.cmake
# --- General configuration ---
set(CMAKE_SYSTEM_NAME RTEMS)
set(CMAKE_SYSTEM_PROCESSOR i386)

# --- Toolchain paths ---
set(RTEMS_PREFIX "/workspaces/rtems-dev")
set(RTEMS_PREFIX_BIN "${RTEMS_PREFIX}/compiler/i386/5/bin")
set(RTEMS_PREFIX_LIB "${RTEMS_PREFIX}/rtems-lib/i386/5/i386-rtems5/pc386/lib")

# --- Compilers ---
set(CMAKE_C_COMPILER ${RTEMS_PREFIX_BIN}/i386-rtems5-gcc)
set(CMAKE_CXX_COMPILER ${RTEMS_PREFIX_BIN}/i386-rtems5-g++)

# --- Prevent CMake from trying to run target programs ---
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_C_COMPILER_WORKS 1)

# --- Typical flags for RTEMS ---
set(CMAKE_C_FLAGS_INIT "-B${RTEMS_PREFIX_LIB} -specs bsp_specs")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-qrtems -Wl,-Ttext,0x00100000")

# --- Prevent searching for host system libraries ---
set(CMAKE_FIND_ROOT_PATH ${RTEMS_PREFIX_LIB})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
