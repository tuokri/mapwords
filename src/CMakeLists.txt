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
