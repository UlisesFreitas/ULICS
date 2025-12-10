#include "cartridge/GameLoader.h"
#include "cartridge/CartridgeLoader.h"
#include "scripting/LuaGame.h"
#include "scripting/ScriptingManager.h"
#include "core/Engine.h"
#include <filesystem>
#include <iostream>

GameLoader::GameLoader(Engine* engine) : engineInstance(engine) {}

std::unique_ptr<LuaGame> GameLoader::loadAndInitializeGame(Engine* engine, const std::string& cartId) {
    // This code can run on a background thread.

    // 1. Construct the full path to the cartridge.
    std::filesystem::path cartPath = std::filesystem::path(engine->getUserDataPath()) / "cartridges" / cartId;

    // 2. Load the cartridge data (config.json, main.lua) from disk.
    std::unique_ptr<Cartridge> cartridge = CartridgeLoader::loadCartridge(cartPath.string());
    if (!cartridge) {
        // Loading failed, return a null game.
        std::cerr << "GameLoader Error: Failed to load cartridge data for '" << cartId << "'." << std::endl;
        return nullptr;
    }

    // 3. Create the scripting environment and the game object.
    // This is the heavy part that used to block the main thread.
    try {
        auto scriptingManager = std::make_unique<ScriptingManager>(engine);

        const auto& config = cartridge->config;
        size_t lineLimit = config.value("/config/lua_code_limit_lines"_json_pointer, 0);

        if (!scriptingManager->LoadAndRunScript(cartridge->luaScript.c_str(), lineLimit)) {
            std::cerr << "GameLoader Error: Failed to load or run script for '" << cartId << "'." << std::endl;
            // The specific error is already logged by ScriptingManager.
            return nullptr;
        }

        // The LuaGame constructor calls the script's _init function.
        // We pass ownership of the cartridge and the scripting manager to the new game object.
        auto luaGame = std::make_unique<LuaGame>(std::move(cartridge), std::move(scriptingManager));
        
        std::cout << "GameLoader: Successfully prepared game '" << cartId << "'." << std::endl;
        return luaGame;

    } catch (const std::exception& e) {
        // Handle exceptions during loading.
        std::cerr << "GameLoader Error: Exception during background game load for '" << cartId << "': " << e.what() << std::endl;
        return nullptr;
    }
}


std::future<std::unique_ptr<LuaGame>> GameLoader::loadGameAsync(const std::string& cartId) {
    // Launch the static helper function asynchronously.
    return std::async(std::launch::async, &GameLoader::loadAndInitializeGame, engineInstance, cartId);
}
