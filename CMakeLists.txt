cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(LevelArchitect VERSION 0.1.0)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define the GEODE_SDK environment variable to point to your Geode installation.")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

include($ENV{GEODE_SDK}/CMakeShared.cmake)

setup_geode_mod(${PROJECT_NAME})

# GmdImporter needs zlib for inflate(). Cocos2d-x/GD likely already links
# a zlib somewhere in the dependency chain - if find_package below fails
# or you get duplicate-symbol errors at link time, remove this block and
# try linking against whatever zlib Geode's own SDK already exposes first.
find_package(ZLIB)
if (ZLIB_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
else()
    message(WARNING "zlib not found via find_package - GmdImporter will fail to link until this is resolved")
endif()
