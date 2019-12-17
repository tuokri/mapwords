include_directories(
        hash
        hashmap
        util
)

add_executable(
        mapwords
        main.c
        hash/hash.c
        hashmap/hashmap.c
        util/util.c
)

add_executable(
        mapwords_debug
        main.c
        hash/hash.c
        hashmap/hashmap.c
        util/util.c
)

target_link_libraries(mapwords m)
target_link_libraries(mapwords_debug m)

target_compile_options(mapwords_debug PUBLIC -DDEBUG)
