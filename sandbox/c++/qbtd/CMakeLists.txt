cmake_minimum_required(VERSION 2.6)

project(QBTD)
set(QBTD_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${QBTD_MODULE_PATH})

find_package(Qt4 REQUIRED QtNetwork)
find_package(Boost REQUIRED system)
find_package(LibtorrentRasterbar REQUIRED)

include(${QT_USE_FILE})
include_directories(${BOOST_INCLUDE_DIRS} ${LIBTORRENT_RASTERBAR_INCLUDE_DIRS})

add_definitions("-DBOOST_FILESYSTEM_VERSION=2")

set(QBTD_SOURCES "src/main.cpp" "src/inspector.hpp" "src/inspector.cpp" "src/torrentsession.hpp" "src/torrentsession_p.hpp" "src/torrentsession.cpp" "src/controlserver.hpp" "src/controlserver_p.hpp" "src/controlserver.cpp" "src/controlsession.hpp" "src/controlsession_p.hpp" "src/controlsession.cpp")

qt4_wrap_cpp(QBTD_MOC_SOURCES "src/inspector.hpp" "src/controlserver_p.hpp" "src/controlsession.hpp" "src/controlsession_p.hpp")

add_executable(qbtd ${QBTD_SOURCES} ${QBTD_MOC_SOURCES})
target_link_libraries(qbtd ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${Boost_LIBRARIES} ${LIBTORRENT_RASTERBAR_LIBRARIES})
set_target_properties(qbtd PROPERTIES COMPILE_FLAGS "-std=c++0x -pipe -pedantic")
