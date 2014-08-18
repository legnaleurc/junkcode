cmake_minimum_required(VERSION 2.8)

find_package(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)
find_package(Boost REQUIRED COMPONENTS coroutine system context)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories("${CMAKE_SOURCE_DIR}/include")

set(SOURCES "include/qtcoroutine.hpp" "src/qtcoroutine.cpp" "src/qtcoroutine_p.hpp")

add_library(qtcoroutine SHARED ${SOURCES})
qt5_use_modules(qtcoroutine Core)
target_link_libraries(qtcoroutine ${Boost_LIBRARIES})

add_executable(example "examples/main.cpp")
qt5_use_modules(example Core)
target_link_libraries(example qtcoroutine)
