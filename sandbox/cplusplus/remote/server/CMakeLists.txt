find_package(Qt5Network REQUIRED)

set(CMAKE_AUTOMOC ON)

set(SOURCES
    "src/main.cpp"
    "src/server.hpp"
    "src/server.cpp"
)

add_executable(server ${SOURCES})
target_link_libraries(server Qt5::Network)
