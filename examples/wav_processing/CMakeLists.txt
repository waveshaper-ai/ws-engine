cmake_minimum_required(VERSION 3.12)
project(wav_processor VERSION 1.0.0 LANGUAGES CXX)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Include directories
include_directories(
    "include"
    "include/${OS_DIR_NAME}"
)

# Source files
set(WAV_PROCESSOR_SOURCES
    "src/main.cpp"
    "src/StreamManager.cpp"
    "src/HannFilter.cpp"
    "src/WavReader.cpp"
)

# Create the executable target
add_executable(${PROJECT_NAME} ${WAV_PROCESSOR_SOURCES})

# Link libraries
target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_NAMES})

# Copy version.txt file to build directory for all platforms
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/version.txt"
        "${CMAKE_BINARY_DIR}/version.txt"
    COMMENT "Copying version.txt to build directory"
)

# Copy DLLs to build directory on Windows
if(WIN32)
    # Copy to the build directory (for Debug builds)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/lib/windows/"
            "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/"
        COMMENT "Copying DLLs to build directory"
    )
endif()

# Handle RPATH for Unix systems to find the libraries at runtime
if(APPLE)
    # Set RPATH to include both the lib directory and $ORIGIN/../lib/linux
    # This allows the executable to find libraries both during development and after installation
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # Use $ORIGIN to find libraries relative to the executable
        INSTALL_RPATH "@loader_path/../lib/${OS_DIR_NAME}" )
else()

    if(UNIX)
        # Set RPATH to include both the lib directory and $ORIGIN/../lib/linux
        # This allows the executable to find libraries both during development and after installation
        set_target_properties(${PROJECT_NAME} PROPERTIES
            # Use $ORIGIN to find libraries relative to the executable
            INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib/linux:${CMAKE_SOURCE_DIR}/lib/linux"
            BUILD_WITH_INSTALL_RPATH TRUE
            # Add the build directory to RPATH for development
            BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/linux"
            # Skip the build tree RPATH in the install RPATH
            SKIP_BUILD_RPATH FALSE
        )


    endif()
endif()

# Print helpful information after build completes
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "Executable created at: ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Run with: ./wav_processor input.wav output.wav -m path/to/model"
)
