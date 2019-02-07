enable_testing()

find_package(Check REQUIRED)
find_package(Threads REQUIRED)

set(
    LIBS
    ${LIBS}
    ${CHECK_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

include_directories(
    ${CHECK_INCLUDE_DIRS}
    .
    ../src/hash
    ../src/hashmap
)

add_executable(
    test_hash
    test_hash.c
    ../src/hash/hash.c
    ../src/hashmap/hashmap.c
)

target_link_libraries(test_hash ${LIBS})

add_test(test_hash ${CMAKE_CURRENT_BINARY_DIR}/test)
