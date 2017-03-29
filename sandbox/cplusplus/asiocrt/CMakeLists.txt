cmake_minimum_required(VERSION 3.6)

project(asiocrt)

find_package(Boost REQUIRED COMPONENTS system coroutine)
find_package(Threads REQUIRED)

set(SOURCES "src/main.cpp" "src/coroutine.cpp")

add_executable(asiocrt ${SOURCES})
target_compile_features(asiocrt PRIVATE cxx_auto_type)
target_link_libraries(asiocrt
    Boost::dynamic_linking
    Boost::disable_autolinking
    Boost::system
    Boost::coroutine
    Threads::Threads)
