cmake_minimum_required(VERSION 3.12)

project(driveclient)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
set(CMAKE_AUTOMOC ON)


set(SOURCES
    "src/main.cpp"
    "src/drivemodel.cpp"
    "src/fileinfogatherer.cpp"
    "src/drivefileinfo.cpp"
    "src/drivesystem.cpp"
)


add_executable(driveclient ${SOURCES})
target_link_libraries(driveclient Qt5::Widgets  Qt5::Network)
