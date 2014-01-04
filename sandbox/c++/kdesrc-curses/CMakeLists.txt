cmake_minimum_required(VERSION 2.8)

project(KDEsrcCurses)

find_package(Curses REQUIRED)
find_package(Boost REQUIRED)

file(GLOB_RECURSE KDEsrcCurses_PUBLIC_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.hpp")
file(GLOB_RECURSE KDEsrcCurses_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE KDEsrcCurses_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

include_directories(${CURSES_INCLUDE_DIR} ${Boost_INCLUDE_DIR} "include" "src")

add_executable("kdesrc-curses" ${KDEsrcCurses_SOURCES} ${KDEsrcCurses_HEADERS} ${KDEsrcCurses_PUBLIC_HEADERS})
target_link_libraries("kdesrc-curses" ${CURSES_LIBRARY} ${Boost_LIBRARIES})
set_target_properties("kdesrc-curses" PROPERTIES COMPILE_FLAGS "-std=c++11 -stdlib=libc++" LINK_FLAGS "-stdlib=libc++")
