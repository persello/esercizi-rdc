cmake_minimum_required(VERSION 3.5)

project(Esercizi)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output)

add_compile_options(-Wall -Wpedantic)

# Libraries
add_subdirectory(lib/mudpsocketlib)
add_subdirectory(lib/rawsocketlib)
add_subdirectory(lib/tcpsocketlib)
add_subdirectory(lib/udpsocketlib)

# Executables
add_subdirectory(NSD01/1_asta_telematica)