cmake_minimum_required(VERSION 3.6)

project(socks5_proxy)

find_package(Boost REQUIRED COMPONENTS system)

set(SOURCES "src/main.cpp")

add_executable(socks5_proxy ${SOURCES})
target_compile_features(socks5_proxy PRIVATE cxx_auto_type)
target_link_libraries(socks5_proxy Boost::system)
