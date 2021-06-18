project(gara_podistica_parallelo)

# Server
add_executable(${PROJECT_NAME}_server server.c)
target_link_libraries( ${PROJECT_NAME}_server
    socketlib::tcp
    liblog::log
    ringbuffer::ringbuffer
)

install(TARGETS ${PROJECT_NAME}_server DESTINATION NSD01)