project(mudpsocket)

add_library(${PROJECT_NAME} src/mudpsocketlib.c)
add_library(socketlib::mudp ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
)