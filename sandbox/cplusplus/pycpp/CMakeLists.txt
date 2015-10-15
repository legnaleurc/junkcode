cmake_minimum_required(VERSION 2.8)

project(pycpp)

find_package(PythonLibs)
find_package(Boost REQUIRED python)

set(PYCPP_COMPILE_UNITS "pycpp.cpp" "Base.cpp" "Base.hpp" "Derived.cpp" "Derived.hpp")

include_directories(${PYTHON_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
add_library(pycpp SHARED ${PYCPP_COMPILE_UNITS})
target_link_libraries(pycpp ${PYTHON_LIBRARY} ${Boost_LIBRARIES})
set_target_properties(pycpp PROPERTIES PREFIX "")
if(APPLE)
	set_target_properties(pycpp PROPERTIES SUFFIX ".so")
endif()
