# --- Project Definition ---
cmake_minimum_required(VERSION 3.16)
project(UliCS LANGUAGES CXX)

# Set modern CMake policies to avoid warnings and ensure robust behavior.
# CMP0135: The DOWNLOAD_EXTRACT_TIMESTAMP option for FetchContent_Declare()
#          is now respected. Setting this to NEW uses the time of extraction.
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# --- Compiler Settings ---
# Request C++20 standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Windows-specific: C Runtime Library ---
# To resolve LNK4098 warnings, we enforce the same C runtime library for all
# targets (our executable, SDL, Lua). We choose the static multithreaded
# runtime (/MT) to avoid dependencies on the Visual C++ Redistributable DLLs.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --- Build SDL2 from source as a static library ---
# This assumes the SDL source code is in a directory named 'sdl'
# at the same level as this CMakeLists.txt file.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sdl/CMakeLists.txt")
    message(FATAL_ERROR "SDL source directory not found at '${CMAKE_CURRENT_SOURCE_DIR}/sdl'. Please clone the SDL repository into the 'sdl' directory.")
endif()

# Set SDL build options BEFORE adding the subdirectory.
# We set them as CACHE variables to pre-configure SDL's options
# and avoid CMake warnings about normal variables being overwritten.
set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library")
set(SDL_SHARED OFF CACHE BOOL "Do not build SDL as a shared library")

# Add the SDL project as a sub-project. CMake will now build SDL for us.
# The 'EXCLUDE_FROM_ALL' option prevents SDL's install targets from running during a normal build.
add_subdirectory(sdl EXCLUDE_FROM_ALL)

# --- Manually managed Lua dependency ---
# We define how to build Lua here, without needing a CMakeLists.txt inside the external/lua directory.

# 1. Find all Lua source files, excluding the command-line interpreter files.
file(GLOB LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/external/lua/*.c")
list(FILTER LUA_SOURCES EXCLUDE REGEX "lua\\.c$|luac\\.c$")

# 2. Create the static library target from those sources.
add_library(lua_lib STATIC ${LUA_SOURCES})

# 3. Set its output name to be clean (e.g., lua.lib).
set_target_properties(lua_lib PROPERTIES OUTPUT_NAME lua)

# 4. Specify that anyone linking against lua_lib should also get its include directory.
target_include_directories(lua_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/lua")

# --- Core Engine Library ---
# We compile all engine code into a static library that can be shared.
add_library(UlicsEngineLib STATIC
    src/core/Engine.cpp src/core/Engine.h
    src/core/FileSystem.cpp src/core/FileSystem.h
    src/core/Constants.h
    src/rendering/AestheticLayer.cpp src/rendering/AestheticLayer.h
    src/game/Game.h
    src/demos/DemoGame.cpp src/demos/DemoGame.h
    src/scripting/LuaGame.cpp src/scripting/LuaGame.h
    src/rendering/EmbeddedFont.h
    src/input/InputManager.cpp src/input/InputManager.h
    src/scripting/ScriptingManager.cpp src/scripting/ScriptingManager.h
    src/cartridge/CartridgeLoader.cpp src/cartridge/CartridgeLoader.h
    src/cartridge/GameLoader.cpp src/cartridge/GameLoader.h
    src/cartridge/Cartridge.h
    src/cartridge/EmbeddedBootCartridge.h
)

# Targets linking against our library need access to the 'src' and 'external' include paths.
target_include_directories(UlicsEngineLib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann_json/include"
)

# Link the engine library against its dependencies.
target_link_libraries(UlicsEngineLib PUBLIC SDL2::SDL2-static PRIVATE SDL2::SDL2main lua_lib)

# --- Main Application Executable ---
# The main executable is now very simple: it's just the entry point.
add_executable(UliCS WIN32 src/main.cpp)

# Link the executable against our engine library.
target_link_libraries(UliCS PRIVATE UlicsEngineLib)

# --- Google Test Integration ---
# Fetch GTest if it's not already present.
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# --- Test Executable Target ---
# Create the test executable.
add_executable(ulics_tests
    tests/CartridgeLoader_test.cpp
)

# Link the test executable against our engine library and GTest.
target_link_libraries(ulics_tests PRIVATE UlicsEngineLib gtest_main)

# Include the GTest source for discovery.
include(GoogleTest)

# --- Build Output ---

# Place both executables in a clean output directory.
set_target_properties(UliCS ulics_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message(STATUS "Project configured. Executable will be named UliCS.exe")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Executable will be generated in: ${CMAKE_BINARY_DIR}/bin")