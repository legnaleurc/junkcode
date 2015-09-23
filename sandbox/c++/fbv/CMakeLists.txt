cmake_minimum_required(VERSION 3.0)

project(fbv)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)


add_executable(fbv "src/main.cpp" "src/keyboardhandler.hpp" "src/keyboardhandler.cpp")

target_link_libraries(fbv Qt5::Widgets)
