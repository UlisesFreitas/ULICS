#ifndef GAME_LOADER_H
#define GAME_LOADER_H

#include <future>
#include <memory>
#include <string>

// Forward declarations
class LuaGame;
class Engine;

/// @brief Holds the results of an asynchronous load operation.
struct AsyncLoadResult {
    std::future<std::unique_ptr<LuaGame>> gameFuture;
    std::shared_ptr<std::atomic<float>> progress;
};


class GameLoader {
public:
    explicit GameLoader(Engine* engine);

    // This is the core function. It starts the background loading process.
    AsyncLoadResult loadGameAsync(const std::string& cartId);

    // Synchronous loader, can be called from anywhere.
    static std::unique_ptr<LuaGame> loadAndInitializeGame(Engine* engine, const std::string& cartId, std::shared_ptr<std::atomic<float>> progress);

private:
    Engine* engineInstance; // Non-owning pointer to the engine
};

#endif // GAME_LOADER_H
