cmake_minimum_required(VERSION 3.12)

project(driveclient)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebSockets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


set(HEADERS
    "src/drivemodel.h"
    "src/drivemodel_p.h"
    "src/fileinfogatherer_p.h"
    "src/drivefileinfo.h"
    "src/drivefileinfo_p.h"
    "src/drivesystem.h"
    "src/drivesystem_p.h"
    "src/drivenode.h"
    "src/mainwindow.h"
    "src/mainwindow_p.h"
)
set(SOURCES
    "src/main.cpp"
    "src/drivemodel.cpp"
    "src/fileinfogatherer.cpp"
    "src/drivefileinfo.cpp"
    "src/drivesystem.cpp"
    "src/drivenode.cpp"
    "src/mainwindow.cpp"
)
set(UIS
    "src/mainwindow.ui"
)


add_executable(driveclient ${SOURCES} ${HEADERS} ${UIS})
target_link_libraries(driveclient Qt5::Widgets Qt5::Network Qt5::WebSockets)
