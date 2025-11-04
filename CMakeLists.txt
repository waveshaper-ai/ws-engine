cmake_minimum_required(VERSION 3.12)
project(WaveshaperCore VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Determine platform-specific settings
set(LIB_NAMES WSutil WSai)

if(WIN32)
    # MSVC naming convention
    set(OS_DIR_NAME "windows")
else()
    if(NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
    endif()

    if(APPLE)
        set(OS_DIR_NAME "macos")
    else()
        set(OS_DIR_NAME "linux")
    endif()
endif()

message("\nBuilding for configuration ${CMAKE_BUILD_TYPE} on platform ${OS_DIR_NAME}.\n")

set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/${OS_DIR_NAME}")
set(PLATFORM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/${OS_DIR_NAME}")

# Include directories
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${PLATFORM_INCLUDE_DIR}"
)

# List below the different examples to be built.
add_subdirectory("examples/wav_processing")
