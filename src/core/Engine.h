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
class AudioManager;  // Audio system (Phase 5.12)
class UISystem;      // Custom UI system (Phase 2.0.1)
class CodeEditor;    // Code editor (Phase 2.0.2)
class SpriteEditor;  // Sprite editor (Phase 3)
class SystemSprites; // System icon sprites
class MenuSystem;    // Menu system
class Settings;      // Settings manager

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(const char* title, int width, int height, const std::string& cartridgePath = "");
    void Run();
    
    // === Engine State Machine ===
    enum class EngineState {
        BOOT,                // Initial boot state
        MAIN_MENU,           // Main menu (native C++)
        PAUSE_MENU,          // Pause menu (during cartridge)
        SETTINGS_MENU,       // Settings/config menu
        LOADING_CARTRIDGE,   // Loading a cartridge
        RUNNING_CARTRIDGE,   // Running a cartridge
        ERROR                // Error state
    };

    EngineState GetState() const { return currentState; }
    void SetState(EngineState newState);

    // === Engine Mode (Phase 2.0.5) ===
    // Controls whether we're in game mode or editor mode
    enum class EngineMode {
        GAME,          // Playing the game (default)
        CODE_EDITOR,   // F1 - Editing code
        SPRITE_EDITOR, // F2 - Editing sprites (future)
        MAP_EDITOR,    // F3 - Editing map (future)
        SFX_EDITOR,    // F4 - Editing SFX (future)
        MUSIC_EDITOR   // F5 - Editing music (future)
    };

    EngineMode GetMode() const { return currentMode; }
    void SetMode(EngineMode newMode);

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
    SystemSprites* getSystemSprites() const { return systemSprites.get(); }
    SpriteEditor* getSpriteEditor() const { return spriteEditor.get(); }
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
    AudioManager* audioManager;  // Audio subsystem (singleton, Phase 5.12 + 1.1.3 fix)

    // UI Systems (Phase 2.0)
    std::unique_ptr<UISystem> uiSystem;      // Custom UI rendering (2.0.1)
    std::unique_ptr<CodeEditor> codeEditor;  // Code editor (2.0.2-2.0.4)
    std::unique_ptr<SpriteEditor> spriteEditor;  // Sprite editor (Phase 3)
    std::unique_ptr<SystemSprites> systemSprites; // System UI icons

    // Menu Systems
    std::unique_ptr<MenuSystem> pauseMenu;      // Pause menu
    std::unique_ptr<MenuSystem> settingsMenu;   // Settings menu
    std::unique_ptr<Settings> settings;         // Global settings

    // State machine
    EngineState currentState;
    EngineState previousState;

    // Mode switching (Phase 2.0.5)
    EngineMode currentMode;

    // Cartridge management
    std::string currentCartridgePath;
};

#endif // ENGINE_H