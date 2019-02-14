include_directories(
    hash
    hashmap
)

add_executable(
    mapwords
    main.c
    hash/hash.c
    hashmap/hashmap.c
)
