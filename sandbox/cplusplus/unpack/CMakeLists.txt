cmake_minimum_required(VERSION 3.0)

find_package(LibArchive REQUIRED)

set(SOURCES
    "src/unpack.cpp")

add_executable(unpack ${SOURCES})
