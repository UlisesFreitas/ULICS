#include "cartridge/GameLoader.h"
#include "cartridge/CartridgeLoader.h"
#include "scripting/LuaGame.h"
#include "scripting/ScriptingManager.h"
#include "core/Engine.h"
#include <filesystem>
#include <iostream>

GameLoader::GameLoader(Engine* engine) : engineInstance(engine) {
}

std::unique_ptr<LuaGame> GameLoader::loadAndInitializeGame(Engine* engine, const std::string& cartId, std::shared_ptr<std::atomic<float>> progress) {
    // This code can run on a background thread.

    // 1. Construct the full path to the cartridge.
    if (progress) progress->store(0.1f); // 10%
    std::filesystem::path cartPath = std::filesystem::path(engine->getUserDataPath()) / "cartridges" / cartId;

    // 2. Load the cartridge data (config.json, main.lua) from disk.
    if (progress) progress->store(0.2f); // 20%
    std::unique_ptr<Cartridge> cartridge = CartridgeLoader::loadCartridge(cartPath.string());
    if (!cartridge) {
        // Loading failed, return a null game.
        std::cerr << "GameLoader Error: Failed to load cartridge data for '" << cartId << "'." << std::endl;
        if (progress) progress->store(1.0f); // Signal completion (even on failure)
        return nullptr;
    }

    if (progress) progress->store(0.5f); // 50%
    // 3. Create the scripting environment and the game object.
    // This is the heavy part that used to block the main thread.
    try {
        auto scriptingManager = std::make_unique<ScriptingManager>(engine);

        const auto& config = cartridge->config;
        size_t lineLimit = config.value("/config/lua_code_limit_lines"_json_pointer, 0);
        if (progress) progress->store(0.7f); // 70%

        if (!scriptingManager->LoadAndRunScript(cartridge->luaScript.c_str(), lineLimit)) {
            std::cerr << "GameLoader Error: Failed to load or run script for '" << cartId << "'." << std::endl;
            // The specific error is already logged by ScriptingManager.
            if (progress) progress->store(1.0f);
            return nullptr;
        }

        // The LuaGame constructor calls the script's _init function.
        // We pass ownership of the cartridge and the scripting manager to the new game object.
        auto luaGame = std::make_unique<LuaGame>(std::move(cartridge), std::move(scriptingManager));
        
        std::cout << "GameLoader: Successfully prepared game '" << cartId << "'." << std::endl;
        if (progress) progress->store(1.0f); // 100%
        return luaGame;

    } catch (const std::exception& e) {
        // Handle exceptions during loading.
        std::cerr << "GameLoader Error: Exception during background game load for '" << cartId << "': " << e.what() << std::endl;
        if (progress) progress->store(1.0f);
        return nullptr;
    }
}


AsyncLoadResult GameLoader::loadGameAsync(const std::string& cartId) {
    auto progress = std::make_shared<std::atomic<float>>(0.0f);

    // Launch the static helper function asynchronously.
    auto future = std::async(std::launch::async, &GameLoader::loadAndInitializeGame, engineInstance, cartId, progress);

    return { std::move(future), progress };
}
