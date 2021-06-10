project(asta_telematica)

# Server
add_executable(${PROJECT_NAME}_server server.c server_utilities.c utilities.c)
target_link_libraries( ${PROJECT_NAME}_server
    socketlib::udp
    liblog::log
)

install(TARGETS ${PROJECT_NAME}_server DESTINATION NSD01)

# Client
add_executable(${PROJECT_NAME}_client client.c client_utilities.c utilities.c)
target_link_libraries( ${PROJECT_NAME}_client
    socketlib::udp
    liblog::log
)

install(TARGETS ${PROJECT_NAME}_client DESTINATION NSD01)
