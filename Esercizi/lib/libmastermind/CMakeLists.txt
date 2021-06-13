project(libmastermind)

add_library(${PROJECT_NAME} src/mastermind_solver.c)
add_library(mastermind::solver ALIAS ${PROJECT_NAME})

target_link_libraries( ${PROJECT_NAME}
    liblog::log
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME}_local_test src/test.c)
target_link_libraries( ${PROJECT_NAME}_local_test
    liblog::log
    mastermind::solver
)

install(TARGETS ${PROJECT_NAME}_local_test DESTINATION NSD01)
