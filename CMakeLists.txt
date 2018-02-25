cmake_minimum_required(VERSION 3.7)
project(projekt1)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES ipk-client.c)
add_executable(projekt1 ${SOURCE_FILES})