cmake_minimum_required(VERSION 2.8.12)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(EXTRA_CXX_FLAGS "-std=c++11")
endif()

find_package(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)
find_package(Boost REQUIRED COMPONENTS coroutine system context)

include_directories("${CMAKE_SOURCE_DIR}/include")

set(SOURCES "include/qtcoroutine.hpp" "src/qtcoroutine.cpp" "src/qtcoroutine_p.hpp")

add_library(qtcoroutine SHARED ${SOURCES})
qt5_use_modules(qtcoroutine Core)
set_target_properties(qtcoroutine PROPERTIES COMPILE_FLAGS ${EXTRA_CXX_FLAGS})
target_link_libraries(qtcoroutine ${Boost_LIBRARIES})

find_package(Qt5Network REQUIRED)
add_executable(example "examples/main.cpp" "examples/httpserver.hpp" "examples/httpserver.cpp")
qt5_use_modules(example Core Network)
set_target_properties(example PROPERTIES COMPILE_FLAGS ${EXTRA_CXX_FLAGS})
target_link_libraries(example qtcoroutine)
