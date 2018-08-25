cmake_minimum_required(VERSION 3.12)

project(driveclient)

find_package(Qt5Widgets REQUIRED)


set(SOURCES
    "src/main.cpp"
)


add_executable(driveclient ${SOURCES})
target_link_libraries(driveclient Qt5::Widgets)
