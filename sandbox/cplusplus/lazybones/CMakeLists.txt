cmake_minimum_required(VERSION 2.6)
project(Lazybones)

add_executable(lazybones lazybones.cpp sourcelist.cpp)
target_link_libraries(lazybones boost_regex)
