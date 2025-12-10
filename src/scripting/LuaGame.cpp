#include "scripting/LuaGame.h"
#include "scripting/ScriptingManager.h"
#include <iostream>

// Include the full definition of AestheticLayer to use its methods.
#include "rendering/AestheticLayer.h"

LuaGame::LuaGame(std::unique_ptr<Cartridge> cart, std::unique_ptr<ScriptingManager> manager) 
    : cartridge(std::move(cart)), scriptingManager(std::move(manager)) {
    if (!scriptingManager) {
        throw std::runtime_error("ScriptingManager provided to LuaGame is null.");
    }
    // The ScriptingManager is already initialized and has loaded the script.
    // Now, call the script's _init function to perform one-time setup.
    std::cout << "LuaGame: Calling _init() on loaded script." << std::endl;
    scriptingManager->CallLuaFunction("_init");
}

bool LuaGame::_update() {
    if (!scriptingManager) return false;
    return scriptingManager->CallLuaFunction("_update");
}

void LuaGame::_draw(AestheticLayer& aestheticLayer) {
    // The aestheticLayer is implicitly available to Lua functions via the upvalue.
    (void)aestheticLayer; // Mark as unused to prevent compiler warnings.
    if (!scriptingManager) return;
    scriptingManager->CallLuaFunction("_draw");
}

const nlohmann::json& LuaGame::getConfig() const {
    return cartridge->config;
}