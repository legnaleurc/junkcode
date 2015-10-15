cmake_minimum_required(VERSION 3.0)

project(CppShell)

set(HEADERS
	"include/cppshell.hpp")
set(SOURCES
	"src/main.cpp"
	"src/cppshell.cpp")

include_directories("include")

add_executable(cppshell ${SOURCES} ${HEADERS})
set_property(TARGET cppshell APPEND PROPERTY
    COMPILE_FLAGS "-std=c++11")
