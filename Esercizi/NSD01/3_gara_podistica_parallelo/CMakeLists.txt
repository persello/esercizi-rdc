project(gara_podistica_parallelo)

# Server
add_executable(${PROJECT_NAME}_server server.c utilities.c)
target_link_libraries( ${PROJECT_NAME}_server
    socketlib::tcp
    liblog::log
    ringbuffer::ringbuffer
    runnerdb::db
)

install(TARGETS ${PROJECT_NAME}_server DESTINATION NSD01)
install(PROGRAMS parallel_check.sh DESTINATION NSD01)