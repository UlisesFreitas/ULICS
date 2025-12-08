#include "scripting/LuaGame.h"
#include "scripting/ScriptingManager.h"
#include <iostream>

// Include the full definition of AestheticLayer to use its methods.
#include "rendering/AestheticLayer.h"

LuaGame::LuaGame(ScriptingManager* manager) : scriptingManager(manager) {
    if (!scriptingManager) {
        throw std::runtime_error("ScriptingManager provided to LuaGame is null.");
    }
    // The ScriptingManager is already initialized and has loaded main.lua in Engine::Initialize.
}

void LuaGame::_update() {
    scriptingManager->CallLuaFunction("_update");
}

void LuaGame::_draw(AestheticLayer& aestheticLayer) {
    // The aestheticLayer is implicitly available to Lua functions via the upvalue.
    (void)aestheticLayer; // Mark as unused to prevent compiler warnings.
    scriptingManager->CallLuaFunction("_draw");
}