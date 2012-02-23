# must use Clang
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
string(REPLACE "-std=c++0x" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE QSNAPSHOT_COCOA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*)

include_directories("include" "src")

add_library(cocoa ${QSNAPSHOT_COCOA_SOURCES})
