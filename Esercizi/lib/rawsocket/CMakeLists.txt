project(rawsocket)

add_library(${PROJECT_NAME} src/rawsocketlib.c)
add_library(socketlib::raw ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
)