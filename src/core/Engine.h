#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include <string>
#include <memory>
#include <chrono>
#include <future>

// Forward declarations
class AestheticLayer;
class Game;
class InputManager;
class GameLoader;
class LuaGame;

/// @brief Defines the possible execution states of the engine.
enum class EngineState {
    Initializing,
    Running,
    Loading,
    Error
};

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(const char* title, int width, int height);
    void Run();
    void RequestCartridgeLoad(const std::string& cartId);
    
    // Public getters for subsystems
    AestheticLayer* getAestheticLayer() const { return aestheticLayer.get(); }
    InputManager* getInputManager() const { return inputManager.get(); }
    GameLoader* getGameLoader() const { return gameLoader.get(); }
    double getElapsedTime() const;
    const std::string& getUserDataPath() const { return userDataPath; }

private:
    // Constants for the fixed timestep game loop.
    static constexpr int UPDATES_PER_SECOND = 60;
    static constexpr double MS_PER_UPDATE = 1000.0 / UPDATES_PER_SECOND;
    
    void enterErrorState(const std::string& message);
    void deployDefaultCartridgeIfNeeded();
    void drawLoadingScreen();
    void drawErrorScreen();
    void Shutdown();

    bool isRunning;
    EngineState currentState;
    std::string userDataPath;
    std::string errorMessage;

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    SDL_Window* window;
    SDL_Renderer* renderer;

    // Core subsystems
    std::unique_ptr<AestheticLayer> aestheticLayer;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<GameLoader> gameLoader;
    
    // Game state
    std::unique_ptr<Game> activeGame;
    std::future<std::unique_ptr<LuaGame>> nextGameFuture;
};

#endif // ENGINE_H