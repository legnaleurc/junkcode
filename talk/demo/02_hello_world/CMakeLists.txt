cmake_minimum_required(VERSION 2.6)

file(GLOB_RECURSE SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")

add_executable(hello ${SOURCES})
