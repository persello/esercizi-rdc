project(ringbuffer)

add_library(${PROJECT_NAME} src/ring_buffer.c)
add_library(ringbuffer::ringbuffer ALIAS ${PROJECT_NAME})

target_link_libraries( ${PROJECT_NAME}
    liblog::log
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)