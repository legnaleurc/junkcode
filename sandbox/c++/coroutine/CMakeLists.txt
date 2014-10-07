cmake_minimum_required(VERSION 2.8.12)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "^Clang|GNU$")
    set(EXTRA_CXX_FLAGS "-std=c++11")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(EXTRA_CXX_FLAGS "-DBOOST_ALL_NO_LIB")
endif()

find_package(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)
find_package(Boost REQUIRED COMPONENTS coroutine system context)

include_directories(${Boost_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include")

set(SOURCES "include/qtcoroutine.hpp" "src/qtcoroutine.cpp" "src/qtcoroutine_p.hpp")

add_library(qtcoroutine SHARED ${SOURCES})
set_target_properties(qtcoroutine PROPERTIES COMPILE_DEFINITIONS QTCOROUTINE_LIBRARY)
qt5_use_modules(qtcoroutine Core)
set_target_properties(qtcoroutine PROPERTIES COMPILE_FLAGS ${EXTRA_CXX_FLAGS})
target_link_libraries(qtcoroutine ${Boost_LIBRARIES})

find_package(Qt5Network REQUIRED)
add_executable(example WIN32 "examples/main.cpp" "examples/httpserver.hpp" "examples/httpserver.cpp")
qt5_use_modules(example Core Network)
set_target_properties(example PROPERTIES COMPILE_FLAGS ${EXTRA_CXX_FLAGS})
target_link_libraries(example qtcoroutine)
if(MSVC)
    target_link_libraries(example Qt5::WinMain)
endif()
