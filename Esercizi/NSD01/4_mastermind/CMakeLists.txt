project(mastermind)

# Client
add_executable(${PROJECT_NAME}_client client.c utilities.c)
target_link_libraries( ${PROJECT_NAME}_client
    socketlib::tcp
    liblog::log
    ringbuffer::ringbuffer
    mastermind::solver
)

install(TARGETS ${PROJECT_NAME}_client DESTINATION NSD01)
