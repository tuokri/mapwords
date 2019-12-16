cmake_minimum_required(VERSION 3.7.2)
project(mapwords C)
enable_testing()

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

include(CheckCCompilerFlag)

check_c_compiler_flag(-msse4.2 SSE4.2_SUPPORTED)
if (SSE4.2_SUPPORTED)
    add_compile_options(-msse4.2)
endif ()
add_compile_options(-Wall -Werror -pedantic)

add_subdirectory(src)

# add_subdirectory(test)
