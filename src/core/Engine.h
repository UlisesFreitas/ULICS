#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include <string>
#include <memory>
#include <chrono>

class AestheticLayer;
class Game;
class ScriptingManager;
class InputManager;
class CartridgeLoader;

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(const char* title, int width, int height);
    void Run();
    
    // Public getters for subsystems
    AestheticLayer* getAestheticLayer() const { return aestheticLayer.get(); }
    InputManager* getInputManager() const { return inputManager.get(); }
    double getElapsedTime() const;

private:
    // Constants for the fixed timestep game loop.
    static constexpr int UPDATES_PER_SECOND = 60;
    static constexpr double MS_PER_UPDATE = 1000.0 / UPDATES_PER_SECOND;
    void enterErrorState(const std::string& message);
    void drawErrorScreen();
    void Shutdown();

    bool isRunning;
    bool inErrorState;
    std::string errorMessage;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<AestheticLayer> aestheticLayer;
    std::unique_ptr<Game> activeGame;
    std::unique_ptr<ScriptingManager> scriptingManager;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<CartridgeLoader> cartridgeLoader;
};

#endif // ENGINE_H