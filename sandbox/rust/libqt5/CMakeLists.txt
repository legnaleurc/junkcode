cmake_minimum_required(VERSION 3.2)

project(libqt5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Qt5Widgets)

add_library(qt5rust SHARED
    "src/rust_qapplication.cpp"
    "src/rust_qlabel.cpp")
target_link_libraries(qt5rust Qt5::Widgets)
# target_compile_features()
