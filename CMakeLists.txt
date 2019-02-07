cmake_minimum_required(VERSION 3.10)
project(mapwords C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

option(BUILD_TEST "Build the unit tests" ON)

add_definitions(-Wall -Werror)

add_subdirectory(src)

if(BUILD_TEST)
    add_subdirectory(test)
endif()
