cmake_minimum_required(VERSION 2.6)

find_package(Boost REQUIRED)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src" "src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include" "include/*.hpp")

include_directories("${CMAKE_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS})

add_executable(json ${SOURCES} ${HEADERS})
set_target_properties(json PROPERTIES COMPILE_FLAGS "-std=c++0x -stdlib=libc++")
