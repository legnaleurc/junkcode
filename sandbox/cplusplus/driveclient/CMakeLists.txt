cmake_minimum_required(VERSION 3.12)

project(driveclient)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)


set(SOURCES
    "src/main.cpp"
    "src/drivemodel.cpp"
    "src/fileinfogatherer.cpp"
)


add_executable(driveclient ${SOURCES})
target_link_libraries(driveclient Qt5::Widgets)
