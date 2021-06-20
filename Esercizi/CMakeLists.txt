cmake_minimum_required(VERSION 3.0.0)
project(Esercizi VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
add_compile_options(-Wall -Wpedantic -Wextra -Wno-unknown-pragmas)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
add_compile_options(-Weverything -Wno-documentation-unknown-command)
endif()

add_compile_options(-Wno-padded -Wno-unused-macros)

# Libraries
add_subdirectory(lib/mudpsocket)
add_subdirectory(lib/rawsocket)
add_subdirectory(lib/tcpsocket)
add_subdirectory(lib/udpsocket)
add_subdirectory(lib/log)
add_subdirectory(lib/ringbuffer)
add_subdirectory(lib/mastermind)

add_subdirectory(NSD01/runner_db)

# Executables
add_subdirectory(NSD01/1_asta_telematica)
add_subdirectory(NSD01/2_gara_podistica)
add_subdirectory(NSD01/3_gara_podistica_parallelo)
add_subdirectory(NSD01/4_mastermind)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
