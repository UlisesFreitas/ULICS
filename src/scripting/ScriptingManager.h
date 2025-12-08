#ifndef SCRIPTING_MANAGER_H
#define SCRIPTING_MANAGER_H

// Incluimos el wrapper de C++ para las cabeceras de la C API de Lua.
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class ScriptingManager {
public:
    ScriptingManager();
    ~ScriptingManager();

private:
    lua_State* L; // Puntero al estado de Lua.
};

#endif // SCRIPTING_MANAGER_H
