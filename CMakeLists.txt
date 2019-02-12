cmake_minimum_required(VERSION 3.7.2)
project(mapwords C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

option(BUILD_TEST "Build the unit tests" ON)

include(CheckCCompilerFlag)

check_c_compiler_flag(-msse4.2 SSE42_SUPPORTED)
if(SSE42_SUPPORTED)
    add_compile_options(-msse4.2)
endif()
add_compile_options(-Wall -Werror -pedantic)

add_subdirectory(src)

if(BUILD_TEST)
    add_subdirectory(test)
endif()
