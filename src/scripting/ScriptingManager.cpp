#include "scripting/ScriptingManager.h"
#include "rendering/AestheticLayer.h"
#include <iostream>

ScriptingManager::ScriptingManager(AestheticLayer* aestheticLayer) 
    : L(nullptr), aestheticLayerInstance(aestheticLayer) {
    // 1. Create a new Lua state.
    L = luaL_newstate();
    if (L) {
        // 2. Open the standard libraries (base, string, math, etc.).
        luaL_openlibs(L);
        std::cout << "ScriptingManager: Estado de Lua creado y librerías abiertas." << std::endl;

        // 3. Register our C++ API in Lua.
        RegisterAPI();
    }
}

ScriptingManager::~ScriptingManager() {
    if (L) {
        lua_close(L);
        std::cout << "ScriptingManager: Estado de Lua cerrado." << std::endl;
    }
}

void ScriptingManager::RegisterAPI() {
    // --- Registering the 'clear' function ---

    // 1. Push a pointer to our AestheticLayer instance onto the Lua stack.
    // This pointer is known as an "upvalue". It's a C value that we associate with a Lua function.
    lua_pushlightuserdata(L, aestheticLayerInstance);

    // 2. Register the C function 'Lua_Clear' with 1 upvalue.
    // The last argument '1' tells Lua to pop one value from the stack (the pointer we just pushed)
    // and associate it with the 'Lua_Clear' function.
    lua_pushcclosure(L, &ScriptingManager::Lua_Clear, 1);

    // 3. Assign the function we just created to a global variable in Lua named "clear".
    lua_setglobal(L, "clear");

    std::cout << "ScriptingManager: Función 'clear' registrada en Lua." << std::endl;
}

int ScriptingManager::Lua_Clear(lua_State* L) {
    // 1. Get the pointer to our AestheticLayer instance from the upvalue.
    AestheticLayer* layer = static_cast<AestheticLayer*>(lua_touserdata(L, lua_upvalueindex(1)));

    // 2. Get the argument (the color) from the Lua stack.
    int colorIndex = luaL_checkinteger(L, 1);

    // 3. Call the actual C++ function.
    layer->Clear(colorIndex);

    // 4. Return the number of values our function returns to Lua (in this case, none).
    return 0;
}
