cmake_minimum_required(VERSION 3.6)

project(logging)

find_package(Boost REQUIRED COMPONENTS log)

set(SOURCES "src/main.cpp")

add_executable(logging ${SOURCES})
target_compile_features(logging PRIVATE cxx_auto_type)
target_compile_definitions(logging PRIVATE BOOST_LOG_DYN_LINK)
target_link_libraries(logging
    Boost::dynamic_linking
    Boost::disable_autolinking
    Boost::log)
