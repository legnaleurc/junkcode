cmake_minimum_required(VERSION 3.7)

project(socks5_proxy)

find_package(Boost REQUIRED COMPONENTS system date_time regex)

set(SOURCES "src/main.cpp")

add_executable(socks5_proxy ${SOURCES})
target_compile_definitions(socks5_proxy PRIVATE "-DBOOST_SYSTEM_DYN_LINK" "-DBOOST_DATE_TIME_DYN_LINK" "-DBOOST_REGEX_DYN_LINK")
target_link_libraries(socks5_proxy Boost::system Boost::date_time Boost::regex)
