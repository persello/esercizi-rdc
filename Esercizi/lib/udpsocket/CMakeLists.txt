project(udpsocket)

add_library(${PROJECT_NAME} src/udpsocketlib.c)
add_library(socketlib::udp ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
)