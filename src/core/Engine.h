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
class Map;
class HotReload;
class DebugConsole;
class GifRecorder;

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(const char* title, int width, int height, const std::string& cartridgePath = "");
    void Run();
    
    // === Engine State Machine ===
    enum class EngineState {
        BOOT,                // Initial boot state
        MENU,                // Showing cartridge selection menu
        LOADING_CARTRIDGE,   // Loading a cartridge
        RUNNING_CARTRIDGE,   // Running a cartridge
        ERROR                // Error state
    };

    EngineState GetState() const { return currentState; }
    void SetState(EngineState newState);

    // === Cartridge Lifecycle Management ===
    // Loads a cartridge from the specified path (directory containing main.lua).
    bool LoadCartridge(const std::string& cartridgePath);

    // Unloads the currently active cartridge.
    void UnloadCartridge();

    // Reloads the current cartridge (useful for development/testing).
    bool ReloadCurrentCartridge();

    // Gets the path of the currently loaded cartridge.
    const std::string& GetCurrentCartridgePath() const { return currentCartridgePath; }
    
    // Public getters for subsystems
    AestheticLayer* getAestheticLayer() const { return aestheticLayer.get(); }
    InputManager* getInputManager() const { return inputManager.get(); }
    Map* getCurrentMap() const { return currentMap.get(); }
    DebugConsole* getDebugConsole() const { return debugConsole.get(); }
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
    std::unique_ptr<Map> currentMap;  // Global map instance
    std::unique_ptr<HotReload> hotReload;  // File watching for hot reload (v1.5.1)
    std::unique_ptr<DebugConsole> debugConsole;  // On-screen debug overlay (v1.5.2)
    std::unique_ptr<GifRecorder> gifRecorder;  // GIF recording (v1.5.4)

    // State machine
    EngineState currentState;
    EngineState previousState;

    // Cartridge management
    std::string currentCartridgePath;
};

#endif // ENGINE_H