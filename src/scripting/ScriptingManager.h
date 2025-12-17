#ifndef SCRIPTING_MANAGER_H
#define SCRIPTING_MANAGER_H

#include <string>
#include <random>

// Include the C++ wrapper for the Lua C API headers.
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class AestheticLayer; // Forward declaration
class InputManager; // Forward declaration
class Engine;       // Forward declaration

class ScriptingManager {
public:
    explicit ScriptingManager(Engine* engine);
    ~ScriptingManager();

    // Loads and runs a Lua script from a string buffer.
    bool LoadAndRunScript(const char* scriptBuffer);

    // Loads and runs a Lua script from an external file.
    bool LoadScriptFromFile(const std::string& filepath);

    // Calls a global Lua function with no arguments or return values.
    // Returns false if an error occurs during the call.
    bool CallLuaFunction(const char* functionName);

    lua_State* GetLuaState() const { return L; }

    const std::string& GetLastLuaError() const { return lastError; }

    // === Resource Tracking (Phase 4 - Task 4.11) ===
    
    // Returns current Lua memory usage in KB
    int GetLuaMemoryUsageKB() const;
    
    // Returns current Lua memory usage in MB
    float GetLuaMemoryUsageMB() const;
    
    // Sets and gets the code line count for the loaded cartridge
    void SetCodeLineCount(int lines);
    int GetCodeLineCount() const;
    
    // Logs resource usage statistics
    void LogResourceStats() const;

private:
    lua_State* L; // Pointer to the Lua state.
    Engine* engineInstance; // Non-owning pointer to the main engine instance.
    std::string lastError;
    std::mt19937 rng; // Mersenne Twister random number generator.
    
    // Resource tracking
    int codeLineCount; // Number of lines in loaded cartridge

    void RegisterAPI();

    // Helper to register a C function with an upvalue.
    void RegisterFunction(const char* luaName, lua_CFunction func);

    // Static bridge function to call AestheticLayer::Clear
    static int Lua_Clear(lua_State* L);

    // Static bridge function to call AestheticLayer::SetPixel
    static int Lua_Pset(lua_State* L);

    // Static bridge function to call AestheticLayer::Line
    static int Lua_Line(lua_State* L);

    // Static bridge function to call AestheticLayer::Rect
    static int Lua_Rect(lua_State* L);

    // Static bridge function to call AestheticLayer::RectFill
    static int Lua_RectFill(lua_State* L);

    // Static bridge function to call AestheticLayer::Circ
    static int Lua_Circ(lua_State* L);

    // Static bridge function to call AestheticLayer::CircFill
    static int Lua_CircFill(lua_State* L);

    // Static bridge function to call AestheticLayer::Pget
    static int Lua_Pget(lua_State* L);

    // Static bridge function to call InputManager::isKeyDown
    static int Lua_Btn(lua_State* L);

    // Static bridge function to call InputManager::isKeyPressed
    static int Lua_Btnp(lua_State* L);

    // Static bridge function to call AestheticLayer::Print
    static int Lua_Print(lua_State* L);

    // Static bridge function to call Engine::getElapsedTime
    static int Lua_Time(lua_State* L);

    // Static bridge function to call AestheticLayer::SetCamera
    static int Lua_Camera(lua_State* L);

    // Static bridge function to call AestheticLayer::SetTransparentColor
    static int Lua_Tcolor(lua_State* L);

    // --- Cartridge Management Functions ---
    // Static bridge function to load a cartridge dynamically
    static int Lua_LoadCartridge(lua_State* L);
    
    // Static bridge function to list available cartridges
    static int Lua_ListCartridges(lua_State* L);

    // --- System Control Functions (Task 4.5.3) ---
    // Exit the engine/application
    static int Lua_Exit(lua_State* L);
    
    // Reset/reload current cartridge
    static int Lua_Reset(lua_State* L);
    
    // Go back to system menu
    static int Lua_GotoMenu(lua_State* L);
    
    // Open Code Editor with cartridge (Phase 2.0.5.6)
    static int Lua_OpenCodeEditor(lua_State* L);

    // --- Sprite Functions (Phase 5.5) ---
    // Draw sprite
    static int Lua_Spr(lua_State* L);
    
    // Draw sprite sheet section (advanced)
    static int Lua_Sspr(lua_State* L);

    // --- Map Functions (Phase 5.9) ---
    // Draw map
    static int Lua_Map(lua_State* L);
    
    // Get tile ID at map coordinates
    static int Lua_Mget(lua_State* L);
    
    // Set tile ID at map coordinates
    static int Lua_Mset(lua_State* L);

    // --- Audio Functions (Phase 5.15) ---
    // Play sound effect
    static int Lua_Sfx(lua_State* L);
    
    // Play/stop music
    static int Lua_Music(lua_State* L);

    // --- Mouse Input Functions (Phase 5.17) ---
    // Get mouse state (returns table with x, y, left, right, middle)
    static int Lua_Mouse(lua_State* L);
    
    // Get mouse X coordinate
    static int Lua_MouseX(lua_State* L);
    
    // Get mouse Y coordinate
    static int Lua_MouseY(lua_State* L);

    // --- Math Functions ---
    static int Lua_Sin(lua_State* L);
    static int Lua_Cos(lua_State* L);
    static int Lua_Atan2(lua_State* L);
    static int Lua_Sqrt(lua_State* L);
    static int Lua_Abs(lua_State* L);
    static int Lua_Flr(lua_State* L);
    static int Lua_Ceil(lua_State* L);
    static int Lua_Rnd(lua_State* L);
};

#endif // SCRIPTING_MANAGER_H
