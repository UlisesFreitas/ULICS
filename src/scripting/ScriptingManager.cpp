#include "scripting/ScriptingManager.h"
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include <iostream>
#include <string> // Required for std::string
#include <array>

ScriptingManager::ScriptingManager(AestheticLayer* aestheticLayer, InputManager* inputManager)
    : L(nullptr), 
      aestheticLayerInstance(aestheticLayer),
      inputManagerInstance(inputManager) {
    // 1. Create a new Lua state.
    L = luaL_newstate();
    if (L) {
        // 2. Open the standard libraries (base, string, math, etc.).
        luaL_openlibs(L);
        std::cout << "ScriptingManager: Lua state created and standard libraries opened." << std::endl;

        // 3. Register our C++ API in Lua.
        RegisterAPI();
    }
}

ScriptingManager::~ScriptingManager() {
    if (L) {
        lua_close(L);
        std::cout << "ScriptingManager: Lua state closed." << std::endl;
    }
}

// Loads and runs a Lua script from the given filepath.
bool ScriptingManager::LoadAndRunScript(const char* scriptBuffer) {
    // luaL_dostring loads and runs a script from a string.
    if (luaL_dostring(L, scriptBuffer) != LUA_OK) {
        // If there was an error, it's on top of the stack.
        const char* error = lua_tostring(L, -1);
        std::cerr << "Error running embedded script: " << error << std::endl;
        lua_pop(L, 1); // Pop the error message from the stack.
        return false;
    }
    std::cout << "ScriptingManager: Successfully loaded and executed embedded script." << std::endl;
    return true;
}

void ScriptingManager::RegisterAPI() {
    RegisterFunction("clear", &ScriptingManager::Lua_Clear);
    RegisterFunction("pset", &ScriptingManager::Lua_Pset);
    RegisterFunction("line", &ScriptingManager::Lua_Line);
    RegisterFunction("rect", &ScriptingManager::Lua_Rect);
    RegisterFunction("rectfill", &ScriptingManager::Lua_RectFill);
    RegisterFunction("circ", &ScriptingManager::Lua_Circ);
    RegisterFunction("circfill", &ScriptingManager::Lua_CircFill);
    RegisterFunction("pget", &ScriptingManager::Lua_Pget);
    RegisterFunction("btn", &ScriptingManager::Lua_Btn);
    RegisterFunction("btnp", &ScriptingManager::Lua_Btnp);
    RegisterFunction("print", &ScriptingManager::Lua_Print);
}

void ScriptingManager::RegisterFunction(const char* luaName, lua_CFunction func) {
    // Push a pointer to this ScriptingManager instance as the upvalue.
    lua_pushlightuserdata(L, this);
    // Create the C-closure with 1 upvalue.
    lua_pushcclosure(L, func, 1);
    // Set the function as a global in Lua.
    lua_setglobal(L, luaName);
    std::cout << "ScriptingManager: '" << luaName << "' function registered in Lua." << std::endl;
}

int ScriptingManager::Lua_Clear(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    // 2. Get the argument (the color) from the Lua stack.
    int colorIndex = luaL_checkinteger(L, 1);

    // 3. Call the actual C++ function.
    layer->Clear(colorIndex);

    // 4. Return the number of values our function returns to Lua (in this case, none).
    return 0;
}

int ScriptingManager::Lua_Pset(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    // 2. Get the arguments from the Lua stack.
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int colorIndex = luaL_checkinteger(L, 3);

    // 3. Call the actual C++ function.
    layer->SetPixel(x, y, colorIndex);

    // 4. This function returns no values to Lua.
    return 0;
}

int ScriptingManager::Lua_Line(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int x2 = luaL_checkinteger(L, 3);
    int y2 = luaL_checkinteger(L, 4);
    int colorIndex = luaL_checkinteger(L, 5);

    layer->Line(x1, y1, x2, y2, colorIndex);

    return 0;
}

int ScriptingManager::Lua_Rect(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int colorIndex = luaL_checkinteger(L, 5);

    layer->Rect(x, y, w, h, colorIndex);

    return 0;
}

int ScriptingManager::Lua_RectFill(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int colorIndex = luaL_checkinteger(L, 5);

    layer->RectFill(x, y, w, h, colorIndex);

    return 0;
}

int ScriptingManager::Lua_Circ(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int r = luaL_checkinteger(L, 3);
    int colorIndex = luaL_checkinteger(L, 4);

    layer->Circ(x, y, r, colorIndex);

    return 0;
}

int ScriptingManager::Lua_CircFill(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int r = luaL_checkinteger(L, 3);
    int colorIndex = luaL_checkinteger(L, 4);

    layer->CircFill(x, y, r, colorIndex);

    return 0;
}

int ScriptingManager::Lua_Pget(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);

    // Call the C++ function.
    uint8_t colorIndex = layer->Pget(x, y);

    // Push the returned value onto the Lua stack.
    lua_pushinteger(L, colorIndex);

    // Return 1 to indicate that we are returning one value to Lua.
    return 1;
}

// Defines the mapping from fantasy console button indices to keyboard scancodes.
static const std::array<SDL_Scancode, 6> buttonMapping = {
    SDL_SCANCODE_LEFT,  // Button 0: Left
    SDL_SCANCODE_RIGHT, // Button 1: Right
    SDL_SCANCODE_UP,    // Button 2: Up
    SDL_SCANCODE_DOWN,  // Button 3: Down
    SDL_SCANCODE_Z,     // Button 4: Action 1 (Z)
    SDL_SCANCODE_X      // Button 5: Action 2 (X)
};

int ScriptingManager::Lua_Btn(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    InputManager* input = sm->inputManagerInstance;

    int buttonIndex = luaL_checkinteger(L, 1);
    bool isDown = false;

    if (buttonIndex >= 0 && buttonIndex < buttonMapping.size()) {
        isDown = input->isKeyDown(buttonMapping[buttonIndex]);
    }

    lua_pushboolean(L, isDown);
    return 1; // Return one value (the boolean).
}

int ScriptingManager::Lua_Btnp(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    InputManager* input = sm->inputManagerInstance;

    int buttonIndex = luaL_checkinteger(L, 1);
    bool isPressed = false;

    if (buttonIndex >= 0 && buttonIndex < buttonMapping.size()) {
        isPressed = input->isKeyPressed(buttonMapping[buttonIndex]);
    }

    lua_pushboolean(L, isPressed);
    return 1; // Return one value (the boolean).
}

int ScriptingManager::Lua_Print(lua_State* L) {
    ScriptingManager* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->aestheticLayerInstance;

    // Get arguments from Lua.
    const char* text = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int colorIndex = luaL_checkinteger(L, 4);

    // Call the C++ function.
    layer->Print(text, x, y, colorIndex);

    return 0; // No return values.
}

// Calls a global Lua function with no arguments or return values.
void ScriptingManager::CallLuaFunction(const char* functionName) {
    lua_getglobal(L, functionName); // Get the function from Lua's global scope

    if (lua_isfunction(L, -1)) {
        // Call the function with 0 arguments and 0 return values.
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            std::cerr << "Error calling Lua function '" << functionName << "': " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1); // Pop error message
        }
    } else {
        lua_pop(L, 1); // Pop the non-function value from the stack
    }
}
