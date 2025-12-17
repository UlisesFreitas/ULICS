#include "scripting/ScriptingManager.h"
#include "scripting/SystemScripts.h"  // For MENU_SCRIPT
#include "rendering/AestheticLayer.h"
#include "rendering/Map.h"  // For map API functions (v1.1.4)
#include "ui/DebugConsole.h"  // For debug console integration (v1.5.2)
#include "input/InputManager.h"
#include "input/InputConstants.h"
#include "core/Engine.h"
#include "cartridge/CartridgeLoader.h"
#include "audio/AudioManager.h"  // For sfx() Lua binding
#include <iostream>
#include <string> // Required for std::string
#include <array>
#include <cmath> // For standard math functions
#include <fstream> // For file I/O
#include <sstream> // For stringstream

constexpr double PI = 3.14159265358979323846;

ScriptingManager::ScriptingManager(Engine* engine)
    : L(nullptr), engineInstance(engine), codeLineCount(0) {
    // 1. Create a new Lua state.
    L = luaL_newstate();
    if (L) {
        // Seed the random number generator.
        std::random_device rd;
        rng.seed(rd());

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

bool ScriptingManager::LoadScriptFromFile(const std::string& filepath) {
    // Open the file for reading.
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // v1.1.6: Better error message with suggestions
        lastError = "Could not open file: " + filepath;
        std::cerr << "\n=== ULICS Error ===" << std::endl;
        std::cerr << "File not found: " << filepath << std::endl;
        std::cerr << "\nPossible causes:" << std::endl;
        std::cerr << "  - File doesn't exist in the cartridge folder" << std::endl;
        std::cerr << "  - Typo in filename (check capitalization)" << std::endl;
        std::cerr << "  - File is in wrong directory" << std::endl;
        std::cerr << "\nExpected location: cartridges/<name>/main.lua" << std::endl;
        std::cerr << "==================\n" << std::endl;
        return false;
    }

    // Read the entire file into a string.
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string scriptContent = buffer.str();
    file.close();

    // Execute the script using the existing LoadAndRunScript method.
    if (luaL_dostring(L, scriptContent.c_str()) != LUA_OK) {
        // v1.1.6: Enhanced error with stack trace
        lastError = lua_tostring(L, -1);
        
        std::cerr << "\n=== ULICS Lua Error ===" << std::endl;
        std::cerr << "Script: " << filepath << std::endl;
        std::cerr << "\nError: " << lastError << std::endl;
        
        // Add stack traceback for better debugging
        luaL_traceback(L, L, nullptr, 1);
        const char* traceback = lua_tostring(L, -1);
        if (traceback) {
            std::cerr << "\nStack Trace:" << std::endl;
            std::cerr << traceback << std::endl;
            lua_pop(L, 1);  // Pop traceback
        }
        
        std::cerr << "\nTip: Check line numbers above for the error location" << std::endl;
        std::cerr << "======================\n" << std::endl;
        
        lua_pop(L, 1);  // Pop error message
        return false;
    }

    std::cout << "ScriptingManager: Successfully loaded and executed script from: " << filepath << std::endl;
    return true;
}

void ScriptingManager::RegisterAPI() {
    RegisterFunction("clear", &ScriptingManager::Lua_Clear);
    RegisterFunction("cls", &ScriptingManager::Lua_Clear); // Alias for clear (PICO-8 style)
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
    RegisterFunction("time", &ScriptingManager::Lua_Time);
    RegisterFunction("camera", &ScriptingManager::Lua_Camera);
    RegisterFunction("tcolor", &ScriptingManager::Lua_Tcolor);

    // Math functions
    RegisterFunction("sin", &ScriptingManager::Lua_Sin);
    RegisterFunction("cos", &ScriptingManager::Lua_Cos);
    RegisterFunction("atan2", &ScriptingManager::Lua_Atan2);
    RegisterFunction("sqrt", &ScriptingManager::Lua_Sqrt);
    RegisterFunction("abs", &ScriptingManager::Lua_Abs);
    RegisterFunction("flr", &ScriptingManager::Lua_Flr);
    RegisterFunction("ceil", &ScriptingManager::Lua_Ceil);
    RegisterFunction("rnd", &ScriptingManager::Lua_Rnd);
    
    // --- Cartridge Management ---
    RegisterFunction("load_cartridge", &ScriptingManager::Lua_LoadCartridge);
    RegisterFunction("list_cartridges", &ScriptingManager::Lua_ListCartridges);
    
    // --- System Control ---
    RegisterFunction("exit", &ScriptingManager::Lua_Exit);
    RegisterFunction("reset", &ScriptingManager::Lua_Reset);
    RegisterFunction("goto_menu", &ScriptingManager::Lua_GotoMenu);
    RegisterFunction("open_code_editor", &ScriptingManager::Lua_OpenCodeEditor); // Phase 2.0.5.6
    
    // --- Sprite Functions (Phase 5.5) ---
    RegisterFunction("spr", &ScriptingManager::Lua_Spr);
    RegisterFunction("sspr", &ScriptingManager::Lua_Sspr);
    
    // --- Map Functions (Phase 5.9) ---
    RegisterFunction("map", &ScriptingManager::Lua_Map);
    RegisterFunction("mget", &ScriptingManager::Lua_Mget);
    RegisterFunction("mset", &ScriptingManager::Lua_Mset);
    
    // --- Audio Functions (Phase 5.15) ---
    RegisterFunction("sfx", &ScriptingManager::Lua_Sfx);
    RegisterFunction("music", &ScriptingManager::Lua_Music);
    
    // --- Mouse Input Functions (Phase 5.17) ---
    RegisterFunction("mouse", &ScriptingManager::Lua_Mouse);
    RegisterFunction("mousex", &ScriptingManager::Lua_MouseX);
    RegisterFunction("mousey", &ScriptingManager::Lua_MouseY);
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
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    // 2. Get the argument (the color) from the Lua stack.
    int colorIndex = luaL_checkinteger(L, 1);

    // 3. Call the actual C++ function.
    layer->Clear(colorIndex);

    // 4. Return the number of values our function returns to Lua (in this case, none).
    return 0;
}

int ScriptingManager::Lua_Pset(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

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
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int x2 = luaL_checkinteger(L, 3);
    int y2 = luaL_checkinteger(L, 4);
    int colorIndex = luaL_checkinteger(L, 5);

    layer->Line(x1, y1, x2, y2, colorIndex);

    return 0;
}

int ScriptingManager::Lua_Rect(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int colorIndex = luaL_checkinteger(L, 5);

    layer->Rect(x, y, w, h, colorIndex);

    return 0;
}

int ScriptingManager::Lua_RectFill(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int colorIndex = luaL_checkinteger(L, 5);

    layer->RectFill(x, y, w, h, colorIndex);

    return 0;
}

int ScriptingManager::Lua_Circ(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int r = luaL_checkinteger(L, 3);
    int colorIndex = luaL_checkinteger(L, 4);

    layer->Circ(x, y, r, colorIndex);

    return 0;
}

int ScriptingManager::Lua_CircFill(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int r = luaL_checkinteger(L, 3);
    int colorIndex = luaL_checkinteger(L, 4);

    layer->CircFill(x, y, r, colorIndex);

    return 0;
}

int ScriptingManager::Lua_Pget(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

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

// Gamepad button mapping (Phase 5.18)
static const std::array<int, 6> gamepadButtonMapping = {
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,  // Button 0
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT, // Button 1
    SDL_CONTROLLER_BUTTON_DPAD_UP,    // Button 2
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,  // Button 3
    SDL_CONTROLLER_BUTTON_A,          // Button 4: A (like Z)
    SDL_CONTROLLER_BUTTON_B           // Button 5: B (like X)
};

int ScriptingManager::Lua_Btn(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    InputManager* input = sm->engineInstance->getInputManager();

    int buttonIndex = luaL_checkinteger(L, 1);
    bool isDown = false;

    if (buttonIndex >= 0 && buttonIndex < buttonMapping.size()) {
        // Check keyboard
        isDown = input->isKeyDown(buttonMapping[buttonIndex]);
        
        // Also check gamepad (Phase 5.18)
        if (!isDown && input->getNumGamepads() > 0) {
            isDown = input->isGamepadButtonDown(gamepadButtonMapping[buttonIndex]);
        }
    }

    lua_pushboolean(L, isDown);
    return 1; // Return one value (the boolean).
}

int ScriptingManager::Lua_Btnp(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    InputManager* input = sm->engineInstance->getInputManager();

    int buttonIndex = luaL_checkinteger(L, 1);
    bool isPressed = false;

    if (buttonIndex >= 0 && buttonIndex < buttonMapping.size()) {
        // Check keyboard
        isPressed = input->isKeyPressed(buttonMapping[buttonIndex]);
        
        // Also check gamepad (Phase 5.18)
        if (!isPressed && input->getNumGamepads() > 0) {
            isPressed = input->isGamepadButtonPressed(gamepadButtonMapping[buttonIndex]);
        }
    }

    lua_pushboolean(L, isPressed);
    return 1; // Return one value (the boolean).
}

int ScriptingManager::Lua_Print(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    // Get arguments from Lua.
    const char* text = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int colorIndex = luaL_checkinteger(L, 4);

    // Call the C++ function.
    layer->Print(text, x, y, colorIndex);
    
    // Also send to debug console (v1.5.2)
    if (sm->engineInstance->getDebugConsole()) {
        sm->engineInstance->getDebugConsole()->AddMessage(text);
    }

    return 0; // No return values.
}

int ScriptingManager::Lua_Time(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    double elapsed = sm->engineInstance->getElapsedTime();
    
    lua_pushnumber(L, elapsed);
    return 1; // Return one value (the number).
}

int ScriptingManager::Lua_Camera(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);

    layer->SetCamera(x, y);

    return 0;
}

int ScriptingManager::Lua_Tcolor(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();

    int colorIndex = luaL_checkinteger(L, 1);

    layer->SetTransparentColor(colorIndex);

    return 0;
}


int ScriptingManager::Lua_Sin(lua_State* L) {
    double x = luaL_checknumber(L, 1);
    // PICO-8 convention: input is 0..1, output is sin(-x * 2 * PI)
    lua_pushnumber(L, std::sin(-x * 2.0 * PI));
    return 1;
}

int ScriptingManager::Lua_Cos(lua_State* L) {
    double x = luaL_checknumber(L, 1);
    // PICO-8 convention: input is 0..1, output is cos(-x * 2 * PI)
    lua_pushnumber(L, std::cos(-x * 2.0 * PI));
    return 1;
}

int ScriptingManager::Lua_Atan2(lua_State* L) {
    double dx = luaL_checknumber(L, 1);
    double dy = luaL_checknumber(L, 2);
    // PICO-8 convention: atan2(dx, -dy) and result is 0..1
    double angle = std::atan2(dx, -dy); // Returns -PI to PI
    double normalized = -angle / (2.0 * PI); // Normalize to -0.5 to 0.5
    if (normalized < 0) {
        normalized += 1.0;
    }
    lua_pushnumber(L, normalized);
    return 1;
}

int ScriptingManager::Lua_Sqrt(lua_State* L) {
    double x = luaL_checknumber(L, 1);
    lua_pushnumber(L, std::sqrt(x));
    return 1;
}

int ScriptingManager::Lua_Abs(lua_State* L) {
    double x = luaL_checknumber(L, 1);
    lua_pushnumber(L, std::abs(x));
    return 1;
}

int ScriptingManager::Lua_Flr(lua_State* L) {
    double x = luaL_checknumber(L, 1);
    lua_pushnumber(L, std::floor(x));
    return 1;
}

int ScriptingManager::Lua_Ceil(lua_State* L) {
    double x = luaL_checknumber(L, 1);
    lua_pushnumber(L, std::ceil(x));
    return 1;
}

int ScriptingManager::Lua_Rnd(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    int n = lua_gettop(L);

    if (n == 0) {
        // rnd() -> returns [0, 1)
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        lua_pushnumber(L, dist(sm->rng));
    } else {
        // rnd(max) -> returns [0, max)
        double max_val = luaL_checknumber(L, 1);
        if (max_val < 0) {
            // To match PICO-8 behavior, rnd of a negative number seems to work on the positive range
            max_val = -max_val;
        }
        std::uniform_real_distribution<double> dist(0.0, max_val);
        lua_pushnumber(L, dist(sm->rng));
    }
    return 1;
}

// === Cartridge Management Functions ===

/**
 * @brief Lua function: load_cartridge(path)
 * 
 * Dynamically loads a cartridge from the specified path.
 * This is used by the system menu to load selected cartridges.
 * 
 * @param path String - Path to cartridge (directory or .lua file)
 * @return boolean - true if loaded successfully, false otherwise
 */
int ScriptingManager::Lua_LoadCartridge(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    // Get cartridge path argument
    const char* path = luaL_checkstring(L, 1);
    
    std::cout << "Lua: load_cartridge called with path: " << path << std::endl;
    
    // Call Engine::LoadCartridge
    bool success = sm->engineInstance->LoadCartridge(std::string(path));
    
    // Return success status to Lua
    lua_pushboolean(L, success);
    return 1;
}

/**
 * @brief Lua function: list_cartridges()
 * 
 * Returns a table of available cartridges in the cartridges/ directory.
 * Used by the system menu to display available games.
 * 
 * OPTIMIZED: Does NOT parse config.json for performance.
 * Returns only name (from directory/filename) and path.
 * 
 * @return table - Array of cartridge info tables with fields: name, path
 */
int ScriptingManager::Lua_ListCartridges(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    // Create a CartridgeLoader instance
    CartridgeLoader loader;
    
    // Scan cartridges directory
    std::string cartridgesDir = "./cartridges";
    auto cartridges = loader.ListAvailableCartridges(cartridgesDir);
    
    std::cout << "Lua: list_cartridges found " << cartridges.size() << " cartridges" << std::endl;
    
    // Create Lua table
    lua_newtable(L);
    
    for (size_t i = 0; i < cartridges.size(); i++) {
        const auto& cart = cartridges[i];
        
        // Create table for this cartridge
        lua_newtable(L);
        
        // Set name field (from directory/filename, NOT from config)
        lua_pushstring(L, "name");
        lua_pushstring(L, cart.name.c_str());
        lua_settable(L, -3);
        
        // Set path field
        lua_pushstring(L, "path");
        lua_pushstring(L, cart.path.c_str());
        lua_settable(L, -3);
        
        // Author field - empty for now (avoid slow config parsing)
        lua_pushstring(L, "author");
        lua_pushstring(L, "");
        lua_settable(L, -3);
        
        // Add to main table (1-indexed in Lua)
        lua_rawseti(L, -2, i + 1);
    }
    
    return 1;
}

// Calls a global Lua function with no arguments or return values.
bool ScriptingManager::CallLuaFunction(const char* functionName) {
    lua_getglobal(L, functionName); // Get the function from Lua's global scope

    if (lua_isfunction(L, -1)) {
        // Call the function with 0 arguments and 0 return values.
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            lastError = lua_tostring(L, -1);
            
            // Enhanced error reporting (v1.1.2 debugging)
            std::cerr << "\n=== ULICS Lua Error ===" << std::endl;
            std::cerr << "Function: " << functionName << std::endl;
            std::cerr << "Error: " << lastError << std::endl;
            
            // Try to get stack trace
            lua_getglobal(L, "debug");
            if (lua_istable(L, -1)) {
                lua_getfield(L, -1, "traceback");
                if (lua_isfunction(L, -1)) {
                    lua_call(L, 0, 1);
                    const char* traceback = lua_tostring(L, -1);
                    if (traceback) {
                        std::cerr << "Stack Trace:\n" << traceback << std::endl;
                    }
                    lua_pop(L, 1);  // Pop traceback
                }
                lua_pop(L, 1);  // Pop debug table
            }
            
            std::cerr << "======================\n" << std::endl;
            
            lua_pop(L, 1); // Pop error message
            return false;
        }
    } else {
        lua_pop(L, 1); // Pop the non-function value from the stack
    }
    return true;
}

// === Resource Tracking Implementation (Task 4.11) ===

int ScriptingManager::GetLuaMemoryUsageKB() const {
    if (!L) return 0;
    
    // lua_gc with LUA_GCCOUNT returns memory in KB
    int memKB = lua_gc(L, LUA_GCCOUNT, 0);
    return memKB;
}

float ScriptingManager::GetLuaMemoryUsageMB() const {
    return GetLuaMemoryUsageKB() / 1024.0f;
}

void ScriptingManager::SetCodeLineCount(int lines) {
    codeLineCount = lines;
}

int ScriptingManager::GetCodeLineCount() const {
    return codeLineCount;
}

void ScriptingManager::LogResourceStats() const {
    std::cout << "=== ULICS Resource Statistics ===" << std::endl;
    std::cout << "  Lua Memory Usage: " << GetLuaMemoryUsageMB() << " MB ("
              << GetLuaMemoryUsageKB() << " KB)" << std::endl;
    std::cout << "  Code Lines Loaded: " << codeLineCount << " lines" << std::endl;
    std::cout << "=================================" << std::endl;
}

// === System Control Functions (Task 4.5.3) ===

/**
 * @brief Lua function: exit()
 * 
 * Exits the ULICS application gracefully.
 */
int ScriptingManager::Lua_Exit(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    std::cout << "Lua: exit() called - shutting down ULICS..." << std::endl;
    
    // Signal the engine to stop running
    // We access the engine through the upvalue pointer
    if (sm && sm->engineInstance) {
        // The engine's Run() loop checks isRunning, but we don't have direct access
        // Instead, we'll use SDL_Quit event
        SDL_Event quitEvent;
        quitEvent.type = SDL_QUIT;
        SDL_PushEvent(&quitEvent);
    }
    
    return 0;
}

/**
 * @brief Lua function: reset()
 * 
 * Resets/reloads the current cartridge.
 */
int ScriptingManager::Lua_Reset(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    std::cout << "Lua: reset() called - reloading current cartridge..." << std::endl;
    
    if (sm && sm->engineInstance) {
        // Call Engine's ReloadCurrentCartridge method
        bool success = sm->engineInstance->ReloadCurrentCartridge();
        lua_pushboolean(L, success);
        return 1;
    }
    
    lua_pushboolean(L, false);
    return 1;
}

/**
 * @brief Lua function: goto_menu()
 * 
 * Returns to the ULICS system menu.
 */
int ScriptingManager::Lua_GotoMenu(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    std::cout << "Lua: goto_menu() called - returning to system menu..." << std::endl;
    
    if (sm && sm->engineInstance) {
        // Unload current cartridge
        sm->engineInstance->UnloadCartridge();
        
        // Set state to MENU
        sm->engineInstance->SetState(Engine::EngineState::MENU);
        
        // Load the menu script
        bool success = sm->LoadAndRunScript(SystemScripts::MENU_SCRIPT.c_str());
        
        lua_pushboolean(L, success);
        return 1;
    }
    
    lua_pushboolean(L, false);
    return 1;
}

/**
 * @brief Lua function: open_code_editor(path)
 * 
 * Opens the Code Editor with the specified cartridge loaded.
 * Unlike load_cartridge(), this does NOT run the game - it opens the editor.
 * User can then press Ctrl+R to run if desired.(Phase 2.0.5.6)
 * 
 * @param path String - Path to cartridge
 * @return boolean - true if loaded successfully, false otherwise
 */
int ScriptingManager::Lua_OpenCodeEditor(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    const char* path = luaL_checkstring(L, 1);
    
    std::cout << "Lua: open_code_editor() called with path: " << path << std::endl;
    
    if (sm && sm->engineInstance) {
        // Load the cartridge (loads files)
        bool success = sm->engineInstance->LoadCartridge(std::string(path));
        
        if (success) {
            // Switch to Code Editor mode instead of running the game
            sm->engineInstance->SetMode(Engine::EngineMode::CODE_EDITOR);
            std::cout << "Lua: Switched to CODE_EDITOR mode" << std::endl;
        }
        
        lua_pushboolean(L, success);
        return 1;
    }
    
    lua_pushboolean(L, false);
    return 1;
}

// === Sprite API Implementation (Phase 5.5) ===

/**
 * @brief Lua function: spr(n, x, y, [w], [h], [flip_x], [flip_y])
 * 
 * Draws a sprite or grid of sprites.
 * Parameters:
 *   - n: sprite ID
 *   - x, y: screen position  
 *   - w, h: width/height in tiles (default 1)
 *   - flip_x, flip_y: flip flags (default false)
 */
int ScriptingManager::Lua_Spr(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    int argc = lua_gettop(L);
    if (argc < 3) {
        return 0; // Need at least n, x, y
    }
    
    int spriteId = static_cast<int>(lua_tointeger(L, 1));
    int x = static_cast<int>(lua_tointeger(L, 2));
    int y = static_cast<int>(lua_tointeger(L, 3));
    int w = (argc >= 4) ? static_cast<int>(lua_tointeger(L, 4)) : 1;
    int h = (argc >= 5) ? static_cast<int>(lua_tointeger(L, 5)) : 1;
    bool flipX = (argc >= 6) ? lua_toboolean(L, 6) : false;
    bool flipY = (argc >= 7) ? lua_toboolean(L, 7) : false;
    
    if (sm && sm->engineInstance) {
        sm->engineInstance->getAestheticLayer()->DrawSprite(spriteId, x, y, w, h, flipX, flipY);
    }
    
    return 0;
}

/**
 * @brief Lua function: sspr(sx, sy, sw, sh, dx, dy, [dw], [dh])
 * 
 * Draws a section of the sprite sheet.
 * Parameters:
 *   - sx, sy: source position in sprite sheet
 *   - sw, sh: source width/height
 *   - dx, dy: destination screen position
 *   - dw, dh: destination width/height (default = sw, sh)
 */
int ScriptingManager::Lua_Sspr(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    int argc = lua_gettop(L);
    if (argc < 6) {
        return 0; // Need at least sx, sy, sw, sh, dx, dy
    }
    
    int sx = static_cast<int>(lua_tointeger(L, 1));
    int sy = static_cast<int>(lua_tointeger(L, 2));
    int sw = static_cast<int>(lua_tointeger(L, 3));
    int sh = static_cast<int>(lua_tointeger(L, 4));
    int dx = static_cast<int>(lua_tointeger(L, 5));
    int dy = static_cast<int>(lua_tointeger(L, 6));
    int dw = (argc >= 7) ? static_cast<int>(lua_tointeger(L, 7)) : sw;
    int dh = (argc >= 8) ? static_cast<int>(lua_tointeger(L, 8)) : sh;
    
    if (sm && sm->engineInstance) {
        sm->engineInstance->getAestheticLayer()->DrawSpriteSection(sx, sy, sw, sh, dx, dy, dw, dh);
    }
    
    return 0;
}

// === Map API Implementation (Phase 5.9) ===
// Note: Map functionality requires map instance to be stored/managed
// For now, these are placeholder implementations
// TODO: Add Map* member to Engine/AestheticLayer for global map access

/**
 * @brief Lua function: map(mx, my, sx, sy, w, h, [layer])
 * 
 * Draws a portion of the map.
 */
int ScriptingManager::Lua_Map(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    int argc = lua_gettop(L);
    if (argc < 6) {
        return 0; // Need mx, my, sx, sy, w, h at minimum
    }
    
    int mx = static_cast<int>(lua_tointeger(L, 1));
    int my = static_cast<int>(lua_tointeger(L, 2));
    int sx = static_cast<int>(lua_tointeger(L, 3));
    int sy = static_cast<int>(lua_tointeger(L, 4));
    int w = static_cast<int>(lua_tointeger(L, 5));
    int h = static_cast<int>(lua_tointeger(L, 6));
    uint8_t layerMask = (argc >= 7) ? static_cast<uint8_t>(lua_tointeger(L, 7)) : 0xFF;
    
    // Implemented in v1.1.4 - Connect to Engine's currentMap
    if (sm && sm->engineInstance && sm->engineInstance->getCurrentMap()) {
        sm->engineInstance->getAestheticLayer()->DrawMap(
            sm->engineInstance->getCurrentMap(), mx, my, sx, sy, w, h, layerMask);
    }
    
    return 0;
}

/**
 * @brief Lua function: mget(x, y, [layer])
 * 
 * Gets tile ID at map coordinates.
 */
int ScriptingManager::Lua_Mget(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    int argc = lua_gettop(L);
    if (argc < 2) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    int x = static_cast<int>(lua_tointeger(L, 1));
    int y = static_cast<int>(lua_tointeger(L, 2));
    int layer = (argc >= 3) ? static_cast<int>(lua_tointeger(L, 3)) : 0;
    
    // Implemented in v1.1.4 - Connect to Engine's currentMap
    if (sm && sm->engineInstance && sm->engineInstance->getCurrentMap()) {
        uint8_t tile = sm->engineInstance->getCurrentMap()->GetTile(x, y, layer);
        lua_pushinteger(L, tile);
        return 1;
    }
    
    lua_pushinteger(L, 0);
    return 1;
}

/**
 * @brief Lua function: mset(x, y, tile_id, [layer])
 * 
 * Sets tile ID at map coordinates.
 */
int ScriptingManager::Lua_Mset(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    int argc = lua_gettop(L);
    if (argc < 3) {
        return 0;
    }
    
    int x = static_cast<int>(lua_tointeger(L, 1));
    int y = static_cast<int>(lua_tointeger(L, 2));
    uint8_t tileId = static_cast<uint8_t>(lua_tointeger(L, 3));
    int layer = (argc >= 4) ? static_cast<int>(lua_tointeger(L, 4)) : 0;
    
    // Implemented in v1.1.4 - Connect to Engine's currentMap
    if (sm && sm->engineInstance && sm->engineInstance->getCurrentMap()) {
        sm->engineInstance->getCurrentMap()->SetTile(x, y, tileId, layer);
    }
    
    return 0;
}

// === Audio API Implementation (Phase 5.15) ===

/**
 * @brief Lua function: sfx(id, [channel], [offset])
 * 
 * Plays a sound effect.
 * 
 * @param id SFX ID (0-255) or -1 to stop
 * @param channel Channel (0-7) or -1 for auto, or -1 with id=-1 to stop all
 * @param offset Time offset in seconds (optional)
 */
int ScriptingManager::Lua_Sfx(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    int argc = lua_gettop(L);
    if (argc < 1) {
        return 0;
    }
    
    int sfxId = static_cast<int>(lua_tointeger(L, 1));
    int channel = (argc >= 2) ? static_cast<int>(lua_tointeger(L, 2)) : 0;
    
    // Play SFX via AudioManager (Phase 5.12 + Bug 1.1.3 fix)
    AudioManager::getInstance().PlaySFX(channel, sfxId);
    
    (void)sm;
    return 0;
}

/**
 * @brief Lua function: music(pattern_id, [fade_ms], [channel_mask])
 * 
 * Plays or stops music.
 * 
 * @param pattern_id Pattern ID (0-255) or -1 to stop
 * @param fade_ms Fade duration in milliseconds (optional)
 * @param channel_mask Channel mask for which channels to play (optional, default all)
 */
int ScriptingManager::Lua_Music(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    
    int argc = lua_gettop(L);
    if (argc < 1) {
        return 0;
    }
    
    int patternId = static_cast<int>(lua_tointeger(L, 1));
    int fadeMs = (argc >= 2) ? static_cast<int>(lua_tointeger(L, 2)) : 0;
    int channelMask = (argc >= 3) ? static_cast<int>(lua_tointeger(L, 3)) : 0x0F;  // All 4 channels
    
    // TODO Phase 5.14: Implement music playback via AudioManager
    // if (patternId == -1) {
    //     AudioManager::getInstance().StopMusic(fadeMs);
    // } else {
    //     AudioManager::getInstance().PlayMusic(patternId, fadeMs, channelMask);
    // }
    
    (void)sm;
    (void)patternId;
    (void)fadeMs;
    (void)channelMask;
    
    return 0;
}

// === Mouse Input API Implementation (Phase 5.17) ===

/**
 * @brief Lua function: mouse()
 * 
 * Returns a table with mouse state:
 * {x = number, y = number, left = boolean, right = boolean, middle = boolean}
 */
int ScriptingManager::Lua_Mouse(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    InputManager* input = sm->engineInstance->getInputManager();
    
    // Create table
    lua_newtable(L);
    
    // Set x field
    lua_pushinteger(L, input->getMouseX());
    lua_setfield(L, -2, "x");
    
    // Set y field
    lua_pushinteger(L, input->getMouseY());
    lua_setfield(L, -2, "y");
    
    // Set left button (SDL_BUTTON_LEFT = 1)
    lua_pushboolean(L, input->isMouseButtonDown(SDL_BUTTON_LEFT));
    lua_setfield(L, -2, "left");
    
    // Set right button (SDL_BUTTON_RIGHT = 3)
    lua_pushboolean(L, input->isMouseButtonDown(SDL_BUTTON_RIGHT));
    lua_setfield(L, -2, "right");
    
    // Set middle button (SDL_BUTTON_MIDDLE = 2)
    lua_pushboolean(L, input->isMouseButtonDown(SDL_BUTTON_MIDDLE));
    lua_setfield(L, -2, "middle");
    
    return 1;  // Return the table
}

/**
 * @brief Lua function: mousex()
 * 
 * Returns mouse X coordinate in screen space
 */
int ScriptingManager::Lua_MouseX(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    InputManager* input = sm->engineInstance->getInputManager();
    
    lua_pushinteger(L, input->getMouseX());
    return 1;
}

/**
 * @brief Lua function: mousey()
 * 
 * Returns mouse Y coordinate in screen space
 */
int ScriptingManager::Lua_MouseY(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    InputManager* input = sm->engineInstance->getInputManager();
    
    lua_pushinteger(L, input->getMouseY());
    return 1;
}
