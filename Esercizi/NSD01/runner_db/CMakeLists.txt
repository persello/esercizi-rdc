project(runnerdb)

add_library(${PROJECT_NAME} src/database.c)
add_library(runnerdb::db ALIAS ${PROJECT_NAME})

target_link_libraries( ${PROJECT_NAME}
    liblog::log
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
