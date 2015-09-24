cmake_minimum_required(VERSION 3.0)

project(fbv)
set(CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

add_executable(fbv
    "src/main.cpp"
    "src/keyboardhandler.hpp"
    "src/keyboardhandler.cpp"
    "src/filecontroller.hpp"
    "src/filecontroller_p.hpp"
    "src/filecontroller.cpp"
    "src/conductor.hpp"
    "src/conductor.cpp"
    "src/exception.hpp"
    "src/exception.cpp"
    "src/model/filemodel.hpp"
    "src/model/filemodel.cpp"
    "src/model/localfilemodel.hpp"
    "src/model/localfilemodel.cpp"
    "src/model/directory/directorymodel.hpp"
    "src/model/directory/directorymodel.cpp"
    "src/model/single/singlemodel.hpp"
    "src/model/single/singlemodel.cpp"
)
#target_compile_features(fbv PRIVATE cxx_auto_type)
target_compile_options(fbv PRIVATE "-std=c++11")
target_include_directories(fbv PRIVATE "src")
target_link_libraries(fbv Qt5::Widgets)
