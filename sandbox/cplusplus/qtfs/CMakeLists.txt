cmake_minimum_required(VERSION 3.7)

project(qtfs)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5Widgets REQUIRED)


set(SOURCES
    "src/main.cpp"
    "src/model/filesystemmodel.cpp"
    "src/widget/mainwindow.cpp"
    "src/widget/viewer.cpp")
set(WIDGETS
    "src/widget/mainwindow.ui"
    "src/widget/viewer.ui")

add_executable(qtfs ${SOURCES} ${WIDGETS})
target_include_directories(qtfs PRIVATE "src")
target_link_libraries(qtfs Qt5::Widgets)
