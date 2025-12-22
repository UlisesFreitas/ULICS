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

# --- stb_image Header-Only Library (v1.1 - Task 1.1.5) ---
# Download stb_image.h automatically if not present
include(FetchContent)

set(STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")

# Create directory if it doesn't exist
file(MAKE_DIRECTORY "${STB_DIR}")

# Download stb_image.h if not already present
if(NOT EXISTS "${STB_DIR}/stb_image.h")
    message(STATUS "Downloading stb_image.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
        "${STB_DIR}/stb_image.h"
        SHOW_PROGRESS
        STATUS download_status
    )
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download stb_image.h")
    endif()
    message(STATUS "stb_image.h downloaded successfully")
else()
    message(STATUS "stb_image.h already present")
endif()

# Download stb_image_write.h if not already present (for screenshots - v1.5.3)
if(NOT EXISTS "${STB_DIR}/stb_image_write.h")
    message(STATUS "Downloading stb_image_write.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h"
        "${STB_DIR}/stb_image_write.h"
        SHOW_PROGRESS
        STATUS download_status
    )
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download stb_image_write.h")
    endif()
    message(STATUS "stb_image_write.h downloaded successfully")
else()
    message(STATUS "stb_image_write.h already present")
endif()

# Download msf_gif.h if not already present (for GIF recording - v1.5.4)
# Using msf_gif instead of jo_gif - better API and documentation
if(NOT EXISTS "${STB_DIR}/msf_gif.h")
    message(STATUS "Downloading msf_gif.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/notnullnotvoid/msf_gif/master/msf_gif.h"
        "${STB_DIR}/msf_gif.h"
        SHOW_PROGRESS
        STATUS download_status
    )
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(WARNING "Failed to download msf_gif.h - GIF recording will not be available")
    else()
        message(STATUS "msf_gif.h downloaded successfully")
    endif()
else()
    message(STATUS "msf_gif.h already present")
endif()

# Add our executable. The WIN32 flag is important for GUI applications on Windows.
add_executable(UliCS WIN32
    src/main.cpp
    src/core/Engine.cpp src/core/Engine.h
    src/core/Bootstrap.cpp src/core/Bootstrap.h
    src/core/Settings.cpp src/core/Settings.h
    src/core/HotReload.cpp src/core/HotReload.h
    src/ui/DebugConsole.cpp src/ui/DebugConsole.h
    src/ui/UISystem.cpp src/ui/UISystem.h
    src/ui/CodeEditor.cpp src/ui/CodeEditor.h
    src/ui/SpriteEditor.cpp src/ui/SpriteEditor.h
    src/ui/SystemSprites.cpp src/ui/SystemSprites.h
    src/ui/FileExplorer.cpp src/ui/FileExplorer.h
    src/ui/MenuSystem.cpp src/ui/MenuSystem.h
    src/utils/FileDialog.cpp src/utils/FileDialog.h
    src/editor/text/UndoRedoManager.cpp src/editor/text/UndoRedoManager.h
    src/editor/text/TextBuffer.cpp src/editor/text/TextBuffer.h
    src/editor/text/TextSelection.cpp src/editor/text/TextSelection.h
    src/editor/ui/Scrollbar.cpp src/editor/ui/Scrollbar.h
    src/editor/rendering/SyntaxHighlighter.cpp src/editor/rendering/SyntaxHighlighter.h
    src/capture/Screenshot.cpp src/capture/Screenshot.h
    src/capture/GifRecorder.cpp src/capture/GifRecorder.h
    src/rendering/AestheticLayer.cpp src/rendering/AestheticLayer.h
    src/rendering/SpriteSheet.cpp src/rendering/SpriteSheet.h
    src/rendering/Map.cpp src/rendering/Map.h
    src/audio/AudioManager.cpp src/audio/AudioManager.h
    src/audio/SFXSynthesizer.cpp src/audio/SFXSynthesizer.h
    src/audio/MusicPlayer.cpp src/audio/MusicPlayer.h
    src/audio/RingBuffer.h
    src/game/Game.h
    src/demos/DemoGame.cpp src/demos/DemoGame.h
    src/scripting/LuaGame.cpp src/scripting/LuaGame.h
    src/scripting/EmbeddedScripts.h
    src/scripting/SystemScripts.h
    src/rendering/EmbeddedFont.h
    src/input/InputManager.cpp src/input/InputManager.h
    src/scripting/ScriptingManager.cpp src/scripting/ScriptingManager.h
    src/cartridge/CartridgeConfig.h
    src/cartridge/CartridgeLoader.cpp src/cartridge/CartridgeLoader.h
)

# Add the 'src' directory to the include path.
# This allows us to use includes relative to the src root, like #include "core/Engine.h".
# PUBLIC means that any other target linking against UliCS would also inherit this include directory.
target_include_directories(UliCS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Add nlohmann/json header-only library
target_include_directories(UliCS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/json")

# Add stb (stb_image.h) header-only library
target_include_directories(UliCS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")

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