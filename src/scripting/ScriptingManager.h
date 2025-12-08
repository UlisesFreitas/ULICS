#ifndef SCRIPTING_MANAGER_H
#define SCRIPTING_MANAGER_H

#include <string>

// Include the C++ wrapper for the Lua C API headers.
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class AestheticLayer; // Forward declaration
class InputManager;   // Forward declaration

class ScriptingManager {
public:
    ScriptingManager(AestheticLayer* aestheticLayer, InputManager* inputManager);
    ~ScriptingManager();

    // Loads and runs a Lua script from a string buffer.
    bool LoadAndRunScript(const char* scriptBuffer);

    // Calls a global Lua function with no arguments or return values.
    void CallLuaFunction(const char* functionName);

    lua_State* GetLuaState() const { return L; }

private:
    lua_State* L; // Pointer to the Lua state.
    AestheticLayer* aestheticLayerInstance; // Non-owning pointer.
    InputManager* inputManagerInstance;     // Non-owning pointer.

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
};

#endif // SCRIPTING_MANAGER_H
