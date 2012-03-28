cmake_minimum_required(VERSION 2.8)

project(KDEsrcCurses)

find_package(Curses REQUIRED)
find_package(Boost REQUIRED COMPONENTS signals)

file(GLOB_RECURSE KDEsrcCurses_PUBLIC_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.hpp")
file(GLOB_RECURSE KDEsrcCurses_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE KDEsrcCurses_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

include_directories(${CURSES_INCLUDE_DIR} ${Boost_INCLUDE_DIR} "include" "src")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x")

add_executable("kdesrc-curses" ${KDEsrcCurses_SOURCES} ${KDEsrcCurses_HEADERS} ${KDEsrcCurses_PUBLIC_HEADERS})
target_link_libraries("kdesrc-curses" ${CURSES_LIBRARY} ${Boost_LIBRARIES})
