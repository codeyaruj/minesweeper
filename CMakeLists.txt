cmake_minimum_required(VERSION 3.15)
project(Minesweeper VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX-)
    add_compile_options($<$<CONFIG:Release>:/O2>)
else()
    add_compile_options(-Wall -Wextra -pedantic)
    add_compile_options($<$<CONFIG:Release>:-O3>)
    add_compile_options($<$<CONFIG:Debug>:-g>)
    add_compile_options($<$<CONFIG:Debug>:-O0>)
endif()

# Find raylib (use system-installed version)
find_package(raylib QUIET)

if(NOT raylib_FOUND)
    message(STATUS "raylib not found, attempting to fetch...")
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.5
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib)
else()
    message(STATUS "Using system raylib")
endif()

# Source files
set(ENGINE_SOURCES
        src/engine/GameEngine.cpp
)

set(RENDERER_SOURCES
        src/renderer/Renderer.cpp
)

set(CORE_SOURCES
        src/core/Application.cpp
        src/main.cpp
)

# Include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

# Create executable
add_executable(${PROJECT_NAME}
        ${ENGINE_SOURCES}
        ${RENDERER_SOURCES}
        ${CORE_SOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Platform-specific settings
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread dl)
endif()

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Copy resources (if any)
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")