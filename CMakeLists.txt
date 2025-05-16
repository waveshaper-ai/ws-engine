cmake_minimum_required(VERSION 3.12)
project(WaveShaperProcessor VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Run the library setup script before configuring (Linux only)
if(NOT WIN32)
    # For Unix/Linux
    execute_process(
        COMMAND bash "${CMAKE_SOURCE_DIR}/setup_libs.sh"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE SCRIPT_RESULT
    )
    
    if(SCRIPT_RESULT AND NOT SCRIPT_RESULT EQUAL 0)
        message(WARNING "Library setup script did not complete successfully. Build may fail.")
    endif()
endif()

# Determine platform-specific settings
if(WIN32)
    if(MINGW OR CMAKE_COMPILER_IS_GNUCXX)
        # MinGW naming convention
        set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/windows")
        set(LIB_NAMES libWSai.dll.a libTLcore.dll.a libTLscript.dll.a)
        message(STATUS "Configuring for MinGW build")
    else()
        # MSVC naming convention
        set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/windows")
        set(LIB_NAMES WSai TLcore TLscript)
        message(STATUS "Configuring for MSVC build")
    endif()
    set(PLATFORM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/windows")
    set(WAV_PLATFORM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/examples/wav_processing/include/windows")
else()
    set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/linux")
    set(LIB_NAMES TLcore TLscript WSai)
    set(PLATFORM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/unix")
    set(WAV_PLATFORM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/examples/wav_processing/include/unix")
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Include directories
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${PLATFORM_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/examples/wav_processing/include"
    "${WAV_PLATFORM_INCLUDE_DIR}"
)

# Source files
set(WAV_PROCESSOR_SOURCES
    "${CMAKE_SOURCE_DIR}/examples/wav_processing/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/examples/wav_processing/src/StreamManager.cpp"
    "${CMAKE_SOURCE_DIR}/examples/wav_processing/src/HannFilter.cpp"
    "${CMAKE_SOURCE_DIR}/examples/wav_processing/src/WavReader.cpp"
)

# Create the executable target
add_executable(wav_processor ${WAV_PROCESSOR_SOURCES})

# Link libraries
target_link_directories(wav_processor PRIVATE ${LIB_DIR})
target_link_libraries(wav_processor PRIVATE ${LIB_NAMES})

# Copy DLLs to build directory on Windows
if(WIN32)
    # Copy to the build directory (for Debug builds)
    add_custom_command(TARGET wav_processor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/lib/windows"
            "${CMAKE_BINARY_DIR}"
        COMMENT "Copying DLLs to build directory"
    )
    
    # Also copy to Release directory if it exists
    add_custom_command(TARGET wav_processor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_BINARY_DIR}/Release"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/lib/windows"
            "${CMAKE_BINARY_DIR}/Release"
        COMMENT "Copying DLLs to Release directory"
    )
endif()

# Handle RPATH for Unix systems to find the libraries at runtime
if(UNIX)
    # Set RPATH to include both the lib directory and $ORIGIN/../lib/linux
    # This allows the executable to find libraries both during development and after installation
    set_target_properties(wav_processor PROPERTIES
        # Use $ORIGIN to find libraries relative to the executable
        INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib/linux:${CMAKE_SOURCE_DIR}/lib/linux"
        BUILD_WITH_INSTALL_RPATH TRUE
        # Add the build directory to RPATH for development
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/linux"
        # Skip the build tree RPATH in the install RPATH
        SKIP_BUILD_RPATH FALSE
    )
endif()

# Print helpful information after build completes
add_custom_command(TARGET wav_processor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "Executable created at: ${CMAKE_BINARY_DIR}/wav_processor"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Run with: ./wav_processor input.wav output.wav -m path/to/model"
)