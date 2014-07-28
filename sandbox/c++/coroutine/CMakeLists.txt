cmake_minimum_required(VERSION 2.8)

find_package(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)
find_package(Boost REQUIRED COMPONENTS coroutine system context)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCES "src/main.cpp" "src/task.cpp" "src/task.hpp" "src/task_p.hpp")

add_executable(task ${SOURCES})
qt5_use_modules(task Core)
target_link_libraries(task ${Boost_LIBRARIES})
