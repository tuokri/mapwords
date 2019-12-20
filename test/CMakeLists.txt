include_directories(
        ${CMAKE_SOURCE_DIR}/src/hash
        ${CMAKE_SOURCE_DIR}/src/hashmap
        ${CMAKE_SOURCE_DIR}/src/util
)

add_executable(
        run_tests
        run_tests.c
        ${CMAKE_SOURCE_DIR}/src/hash/hash.c
        ${CMAKE_SOURCE_DIR}/src/hashmap/hashmap.c
        ${CMAKE_SOURCE_DIR}/src/util/util.c
)

target_link_libraries(run_tests m)
target_compile_options(run_tests PUBLIC -DDEBUG)
