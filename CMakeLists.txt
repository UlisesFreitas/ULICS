# --- Project Definition ---
cmake_minimum_required(VERSION 3.16)
project(UliCS LANGUAGES CXX)

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

# --- Main Application Target ---
# Add our executable. The WIN32 flag is important for GUI applications on Windows.
add_executable(UliCS WIN32
    src/main.cpp
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
    src/cartridge/EmbeddedBootCartridge.h
)

# Add the 'src' directory to the include path.
# This allows us to use includes relative to the src root, like #include "core/Engine.h".
# PUBLIC means that any other target linking against UliCS would also inherit this include directory.
target_include_directories(UliCS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(UliCS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann_json/include")

# Link our application against the static SDL2 library (SDL2::SDL2-static).
# This target is created by the add_subdirectory(sdl) command above.
# It automatically handles include directories and system library dependencies.
target_link_libraries(UliCS PRIVATE SDL2::SDL2-static SDL2::SDL2main lua_lib)

# --- Build Output ---

# Optional: Place the final executable in a clean directory.
# By default, it goes to build/Debug/ or build/Release/.
set_target_properties(UliCS PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message(STATUS "Project configured. Executable will be named UliCS.exe")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Executable will be generated in: ${CMAKE_BINARY_DIR}/bin")