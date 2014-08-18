cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_MACOSX_RPATH ON)

find_package(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)
find_package(Boost REQUIRED COMPONENTS coroutine system context)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories("${CMAKE_SOURCE_DIR}/include")

set(SOURCES "include/qtcoroutine.hpp" "src/qtcoroutine.cpp" "src/qtcoroutine_p.hpp")

add_library(qtcoroutine SHARED ${SOURCES})
qt5_use_modules(qtcoroutine Core)
target_link_libraries(qtcoroutine ${Boost_LIBRARIES})

find_package(Qt5Network REQUIRED)
add_executable(example "examples/main.cpp" "examples/httpserver.hpp" "examples/httpserver.cpp")
qt5_use_modules(example Core Network)
target_link_libraries(example qtcoroutine)
