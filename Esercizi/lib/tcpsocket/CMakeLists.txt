project(tcpsocket)

add_library(${PROJECT_NAME} src/tcpsocketlib.c)
add_library(socketlib::tcp ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
)