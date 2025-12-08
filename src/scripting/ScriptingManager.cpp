#include "scripting/ScriptingManager.h"
#include <iostream>

ScriptingManager::ScriptingManager() : L(nullptr) {
    // 1. Crear un nuevo estado de Lua.
    L = luaL_newstate();
    if (L) {
        // 2. Abrir las librerías estándar (base, string, math, etc.).
        luaL_openlibs(L);
        std::cout << "ScriptingManager: Estado de Lua creado y librerías abiertas." << std::endl;
    }
}

ScriptingManager::~ScriptingManager() {
    if (L) {
        lua_close(L);
        std::cout << "ScriptingManager: Estado de Lua cerrado." << std::endl;
    }
}
