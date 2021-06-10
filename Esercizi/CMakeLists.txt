cmake_minimum_required(VERSION 3.0.0)
project(Esercizi VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output)

add_compile_options(-Wall -Wpedantic)

# Libraries
add_subdirectory(lib/mudpsocketlib)
add_subdirectory(lib/rawsocketlib)
add_subdirectory(lib/tcpsocketlib)
add_subdirectory(lib/udpsocketlib)
add_subdirectory(lib/liblog)

# Executables
add_subdirectory(NSD01/1_asta_telematica)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
