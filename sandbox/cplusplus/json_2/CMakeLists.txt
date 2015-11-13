cmake_minimum_required(VERSION 2.6)
project(sandbox_5)

add_executable(sandbox_5 main.cpp variant.cpp)
target_compile_features(sandbox_5 PRIVATE cxx_auto_type)

install(TARGETS sandbox_5 RUNTIME DESTINATION bin)
