#ifndef GAME_LOADER_H
#define GAME_LOADER_H

#include <future>
#include <memory>
#include <string>

// Forward declarations
class LuaGame;
class Engine;

class GameLoader {
public:
    explicit GameLoader(Engine* engine);

    // This is the core function. It starts the background loading process.
    std::future<std::unique_ptr<LuaGame>> loadGameAsync(const std::string& cartId);

    // Synchronous loader, can be called from anywhere.
    static std::unique_ptr<LuaGame> loadAndInitializeGame(Engine* engine, const std::string& cartId);

private:
    Engine* engineInstance; // Non-owning pointer to the engine
};

#endif // GAME_LOADER_H
