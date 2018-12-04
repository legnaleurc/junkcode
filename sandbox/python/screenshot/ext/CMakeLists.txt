cmake_minimum_required(VERSION 3.12)

project(screenshot)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFmpeg REQUIRED libavutil libavcodec libavformat libswscale)


set(SOURCES
    "frame.h"
    "frame.c"
    "input.h"
    "input.c"
    "output.h"
    "output.c"
    "screenshot.h"
    "screenshot.c"
)


add_library(screenshot SHARED ${SOURCES})
set_target_properties(screenshot PROPERTIES
    C_STANDARD_REQUIRED ON
    C_STANDARD 11
)
target_compile_options(screenshot PRIVATE "-Wall" "-Werror" "-pedantic")
target_include_directories(screenshot PRIVATE ${FFmpeg_INCLUDE_DIRS})
target_link_libraries(screenshot ${FFmpeg_LIBRARIES})
