cmake_minimum_required(VERSION 3.0)

project(fbv)
set(CXX_EXTENSIONS OFF)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

add_executable(fbv "src/main.cpp" "src/keyboardhandler.hpp" "src/keyboardhandler.cpp")
#target_compile_features(fbv PRIVATE cxx_auto_type)
target_compile_options(fbv PRIVATE "-std=c++11")
target_link_libraries(fbv Qt5::Widgets)
