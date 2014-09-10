cmake_minimum_required(VERSION 2.8.12)

project(Browser)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE FORMS "src/*.ui")
if(WIN32)
    list(APPEND SOURCES "win/winrobot.cpp")
    list(APPEND SOURCES "win/winrobot.h")
    include_directories("win")
endif()

include_directories("${CMAKE_BINARY_DIR}" "src")


find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5WebKit REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5Test REQUIRED)

find_package(Boost REQUIRED COMPONENTS coroutine context system)

find_package(OpenCV REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(${Boost_INCLUDE_DIRS})

set(EXTRA_CXX_FLAGS "-DBOOST_ALL_NO_LIB")
if(CMAKE_COMPILER_IS_GNUCXX)
    set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -std=c++11")
endif()

add_executable(browser WIN32 ${SOURCES} ${HEADERS} resource.qrc)
qt5_use_modules(browser Core Gui WebKit Network Sql Widgets WebKitWidgets Test)
set_target_properties(browser PROPERTIES COMPILE_FLAGS ${EXTRA_CXX_FLAGS})
target_link_libraries(browser ${Boost_LIBRARIES} ${OpenCV_LIBRARIES})
