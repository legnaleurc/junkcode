# root

cmake_minimum_required(VERSION 2.6)

project(UMLEditor)
set(UMLEDITOR_VERSION_MAJOR "0")
set(UMLEDITOR_VERSION_MINOR "0")
set(UMLEDITOR_VERSION_PATCH "0")
set(UMLEDITOR_VERSION "${UMLEDITOR_VERSION_MAJOR}.${UMLEDITOR_VERSION_MINOR}.${UMLEDITOR_VERSION_PATCH}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Qt4 4.7 REQUIRED)

include(${QT_USE_FILE})

add_definitions(-DUMLEDITOR_VERSION=${UMLEDITOR_VERSION} -DUMLEDITOR_VERSION_MAJOR=${UMLEDITOR_VERSION_MAJOR} -DUMLEDITOR_VERSION_MINOR=${UMLEDITOR_VERSION_MINOR} -DUMLEDITOR_VERSION_PATCH=${UMLEDITOR_VERSION_PATCH})
foreach(type "RELEASE" "MINSIZEREL")
    set(CMAKE_CXX_FLAGS_${type} "${CMAKE_CXX_FLAGS_${type}} -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT")
endforeach()
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pedantic -Wextra -Wall -pipe")
elseif(MSVC)
    # general flags
    add_definitions("/D__STDC_CONSTANT_MACROS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t- /wd4127 /wd4819 /MP /nologo")
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
    # release flags
    foreach(type "RELEASE" "MINSIZEREL")
        if(CMAKE_CXX_FLAGS_${type} MATCHES "/O[0-3d]")
            string(REGEX REPLACE "/O[0-3d]" "/Ox" CMAKE_CXX_FLAGS_${type} "${CMAKE_CXX_FLAGS_${type}}")
        else()
            set(CMAKE_CXX_FLAGS_${type} "${CMAKE_CXX_FLAGS_${type}} /Ox")
        endif()
    endforeach()
endif()
set(CMAKE_DEBUG_POSTFIX d)

# hack MSVC path
if(MSVC)
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
else()
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
endif()

file(GLOB_RECURSE UMLEDITOR_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.hpp)
file(GLOB_RECURSE UMLEDITOR_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file(GLOB_RECURSE UMLEDITOR_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.ui)
file(GLOB_RECURSE UMLEDITOR_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.qrc)
include_directories(
    src
    ${CMAKE_CURRENT_BINARY_DIR})
set(UMLEDITOR_MOC_HEADERS "")
foreach(header ${UMLEDITOR_HEADERS})
    file(STRINGS ${header} moc_token REGEX "Q_OBJECT")
    if(moc_token)
        list(APPEND UMLEDITOR_MOC_HEADERS ${header})
    endif()
endforeach()

qt4_wrap_cpp(UMLEDITOR_MOC_SOURCES ${UMLEDITOR_MOC_HEADERS})
qt4_wrap_ui(UMLEDITOR_UIC_HEADERS ${UMLEDITOR_FORMS})
qt4_add_resources(UMLEDITOR_RCC_SOURCES ${UMLEDITOR_RESOURCES})

source_group("Generated Files" FILES ${UMLEDITOR_MOC_SOURCES} ${UMLEDITOR_RCC_SOURCES} ${UMLEDITOR_UIC_HEADERS})
source_group("Resource Files" REGULAR_EXPRESSION .*\\.rc)
source_group("Form Files" REGULAR_EXPRESSION .*\\.ui)

if(MSVC)
    add_executable(umleditor WIN32 ${UMLEDITOR_HEADERS} ${UMLEDITOR_SOURCES} ${UMLEDITOR_MOC_SOURCES} ${UMLEDITOR_UIC_HEADERS} ${UMLEDITOR_RCC_SOURCES})
    target_link_libraries(umleditor ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY})
else()
    add_executable(umleditor ${UMLEDITOR_HEADERS} ${UMLEDITOR_SOURCES} ${UMLEDITOR_MOC_SOURCES} ${UMLEDITOR_UIC_HEADERS} ${UMLEDITOR_RCC_SOURCES})
    target_link_libraries(umleditor ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif()
set_target_properties(umleditor PROPERTIES VERSION ${UMLEDITOR_VERSION})

install(TARGETS umleditor DESTINATION "bin")
