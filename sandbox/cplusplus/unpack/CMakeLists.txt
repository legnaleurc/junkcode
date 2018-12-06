cmake_minimum_required(VERSION 3.0)

find_package(LibArchive REQUIRED)

set(SOURCES
    "src/unpack.cpp"
    "src/main.cpp")

add_executable(unpack ${SOURCES})
target_include_directories(unpack PRIVATE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(unpack ${LibArchive_LIBRARIES})
