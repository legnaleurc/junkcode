cmake_minimum_required(VERSION 3.6)
project(regex_test)

find_package(PythonLibs 3.5.2 REQUIRED)

add_library(regex_ctypes SHARED "src/cplusplus.cpp")

add_library(regex_import SHARED "src/cplusplus.cpp" "src/python.c")
target_include_directories(regex_import PRIVATE ${PYTHON_INCLUDE_DIRS})
set_target_properties(regex_import PROPERTIES PREFIX "")
