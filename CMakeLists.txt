cmake_minimum_required(VERSION 3.7.2)
project(mapwords C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Werror -pedantic)

add_subdirectory(src)
add_subdirectory(test)
