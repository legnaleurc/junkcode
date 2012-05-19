cmake_minimum_required(VERSION 2.6)

project(BigInteger)

find_package(Boost COMPONENTS unit_test_framework)

if(CMAKE_COMPILER_IS_GNUCXX)
    find_package(Boost REQUIRED COMPONENTS regex)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pedantic -Wextra -Wall -pipe")
endif()

file(GLOB_RECURSE BI_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE BI_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.hpp")
file(GLOB_RECURSE BI_PRIVATE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(bi ${BI_SOURCES} ${BI_HEADERS} ${BI_PRIVATE_HEADERS})

if(Boost_FOUND)
    file(GLOB_RECURSE BI_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/*.cpp")
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(tests ${BI_TESTS})
    add_dependencies(tests bi)
    target_link_libraries(tests bi ${Boost_LIBRARIES})
endif()
