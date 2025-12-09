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

    // Loads and runs a Lua script from a string buffer, checking against a line limit.
    bool LoadAndRunScript(const char* scriptBuffer, size_t line_limit);

    // Calls a global Lua function with no arguments or return values.
    // Returns false if an error occurs during the call.
    bool CallLuaFunction(const char* functionName);

    lua_State* GetLuaState() const { return L; }

    const std::string& GetLastLuaError() const { return lastError; }

private:
    lua_State* L; // Pointer to the Lua state.
    Engine* engineInstance; // Non-owning pointer to the main engine instance.
    std::string lastError;
    std::mt19937 rng; // Mersenne Twister random number generator.

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
    static int Lua_TColor(lua_State* L);

    // Static bridge function to scan for and list available cartridges.
    static int Lua_ListCarts(lua_State* L);

    // Static bridge function to request loading a new cartridge.
    static int Lua_LoadCart(lua_State* L);

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
