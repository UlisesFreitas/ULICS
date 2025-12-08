#ifndef SCRIPTING_MANAGER_H
#define SCRIPTING_MANAGER_H

// Include the C++ wrapper for the Lua C API headers.
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class AestheticLayer; // Forward declaration

class ScriptingManager {
public:
    ScriptingManager(AestheticLayer* aestheticLayer);
    ~ScriptingManager();

private:
    lua_State* L; // Pointer to the Lua state.
    AestheticLayer* aestheticLayerInstance; // Pointer to the aesthetic layer we will control.

    void RegisterAPI();

    // Static bridge function to call AestheticLayer::Clear
    static int Lua_Clear(lua_State* L);
};

#endif // SCRIPTING_MANAGER_H
