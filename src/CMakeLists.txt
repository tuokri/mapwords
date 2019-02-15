include_directories(
    hash
    hashmap
    search
    util
)

add_executable(
    mapwords
    main.c
    hash/hash.c
    hashmap/hashmap.c
    search/search.c
    util/util.c
)
