# --- Project Definition ---
cmake_minimum_required(VERSION 3.16)
project(UliCS LANGUAGES CXX)

# --- Compiler Settings ---
# Request C++20 standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Build SDL2 from source as a static library ---
# This assumes the SDL source code is in a directory named 'sdl'
# at the same level as this CMakeLists.txt file.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sdl/CMakeLists.txt")
    message(FATAL_ERROR "SDL source directory not found at '${CMAKE_CURRENT_SOURCE_DIR}/sdl'. Please clone the SDL repository into the 'sdl' directory.")
endif()

# Set SDL build options BEFORE adding the subdirectory.
# This ensures SDL is built as a static library.
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

# Add the SDL project as a sub-project. CMake will now build SDL for us.
# The 'EXCLUDE_FROM_ALL' option prevents SDL's install targets from running during a normal build.
add_subdirectory(sdl EXCLUDE_FROM_ALL)

# --- Main Application Target ---
# Add our executable. The WIN32 flag is important for GUI applications on Windows.
add_executable(UliCS WIN32 src/main.cpp src/Engine.cpp src/Engine.h)

# Link our application against the static SDL2 library (SDL2::SDL2-static).
# This target is created by the add_subdirectory(sdl) command above.
# It automatically handles include directories and system library dependencies.
# SDL2::SDL2main is required on Windows to provide the WinMain entry point.
target_link_libraries(UliCS PRIVATE SDL2::SDL2-static SDL2::SDL2main)

# --- Build Output ---

# Optional: Place the final executable in a clean directory.
# By default, it goes to build/Debug/ or build/Release/.
set_target_properties(UliCS PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message(STATUS "Project configured. Executable will be named UliCS.exe")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Executable will be generated in: ${CMAKE_BINARY_DIR}/bin")