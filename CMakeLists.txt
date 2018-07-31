cmake_minimum_required(VERSION 3.10)
project(mapwords C)

set(CMAKE_C_STANDARD 11)

include_directories(
    common
    hash
    hashmap)

add_executable(
    mapwords
    main.c
    common/glo_def.h
    hash/hash.h
    hash/hash.c)
