project(gara_podistica)

# Server
add_executable(${PROJECT_NAME}_server server.c utilities.c)
target_link_libraries( ${PROJECT_NAME}_server
    socketlib::tcp
    liblog::log
    ringbuffer::ringbuffer
    runnerdb::db
)

install(TARGETS ${PROJECT_NAME}_server DESTINATION NSD01)

# Client
add_executable(${PROJECT_NAME}_client client.c utilities.c)
target_link_libraries( ${PROJECT_NAME}_client
    socketlib::tcp
    liblog::log
    ringbuffer::ringbuffer
)

install(TARGETS ${PROJECT_NAME}_client DESTINATION NSD01)
