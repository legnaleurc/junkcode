cmake_minimum_required(VERSION 2.8.12)

project(Browser)

file(GLOB_RECURSE SOURCES "src" "*.cpp")
file(GLOB_RECURSE HEADERS "src" "*.h")

add_executable(browser WIN32 ${SOURCES} ${HEADERS})


find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5WebKit REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)

qt5_use_modules(browser Core Gui WebKit Network Sql Widgets WebKitWidgets)
