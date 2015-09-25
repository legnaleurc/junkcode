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
    "src/widget/imageitem.hpp"
    "src/widget/imageitem_p.hpp"
    "src/widget/imageitem.cpp"
    "src/model/filemodel.hpp"
    "src/model/filemodel.cpp"
    "src/model/localfilemodel.hpp"
    "src/model/localfilemodel.cpp"
    "src/model/directorymodel.hpp"
    "src/model/directorymodel.cpp"
    "src/model/singlemodel.hpp"
    "src/model/singlemodel.cpp"
    "src/model/archivemodel.hpp"
    "src/model/archivemodel_p.hpp"
    "src/model/archivemodel.cpp"
    "src/loader/deviceloader.hpp"
    "src/loader/deviceloader_p.hpp"
    "src/loader/deviceloader.cpp"
    "src/loader/blockdeviceloader.hpp"
    "src/loader/blockdeviceloader.cpp"
    "src/loader/characterdeviceloader.hpp"
    "src/loader/characterdeviceloader_p.hpp"
    "src/loader/characterdeviceloader.cpp"
    "src/loader/asynchronousloader.hpp"
    "src/loader/asynchronousloader.cpp"
)
#target_compile_features(fbv PRIVATE cxx_auto_type)
target_compile_options(fbv PRIVATE "-std=c++11")
target_include_directories(fbv PRIVATE "src")
target_link_libraries(fbv Qt5::Widgets)
