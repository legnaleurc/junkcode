cmake_minimum_required(VERSION 3.0)

find_package(LibArchive REQUIRED)
find_package(cpprestsdk REQUIRED)
find_package(Boost REQUIRED thread filesystem)

set(SOURCES
    "src/exception.cpp"
    "src/unpack.cpp"
    "src/main.cpp")

add_executable(unpack)
target_compile_features(unpack PUBLIC cxx_std_11)
target_include_directories(unpack PRIVATE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(unpack PRIVATE
    ${LibArchive_LIBRARIES}
    cpprestsdk::cpprest
    Boost::thread
    Boost::filesystem)
target_sources(unpack PRIVATE ${SOURCES})
