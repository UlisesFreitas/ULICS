#include "core/Engine.h"
#include "rendering/AestheticLayer.h"
#include "scripting/LuaGame.h" // Include our Lua game bridge.
#include "input/InputManager.h" // Include the new InputManager.
#include "cartridge/CartridgeLoader.h" // Cartridge system
#include "cartridge/CartridgeConfig.h" // Cartridge configuration
#include <iostream>
#include <fstream>  // For file I/O (code line counting)
#include <sstream>  // For string stream (error screen)
#include <vector>   // For vector (error screen line wrapping)
#include <chrono>
#include "scripting/ScriptingManager.h"
#include "scripting/EmbeddedScripts.h" // Include our new embedded script header.
#include "scripting/SystemScripts.h" // System menu

Engine::Engine() : isRunning(false), inErrorState(false), errorMessage(""),
                   window(nullptr), renderer(nullptr), aestheticLayer(nullptr), 
                   activeGame(nullptr), scriptingManager(nullptr),
                   inputManager(nullptr), cartridgeLoader(nullptr),
                   currentState(EngineState::BOOT), previousState(EngineState::BOOT),
                   currentCartridgePath("") {
    // Constructor
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(const char* title, int width, int height, const std::string& cartridgePath) {
    // Initialize SDL with video and gamecontroller support
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create an accelerated renderer with V-Sync to prevent screen tearing/flickering
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Error creating renderer: " << SDL_GetError() << std::endl;
        Shutdown();
        return false;
    }
    // Maintain aspect ratio when scaling.
    SDL_RenderSetLogicalSize(renderer, AestheticLayer::FRAMEBUFFER_WIDTH, AestheticLayer::FRAMEBUFFER_HEIGHT);

    try {
        aestheticLayer = std::make_unique<AestheticLayer>(renderer);
    } catch (const std::exception& e) {
        std::cerr << "Error initializing AestheticLayer: " << e.what() << std::endl;
        return false;
    }

    try {
        inputManager = std::make_unique<InputManager>();
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing InputManager: " << e.what() << std::endl;
        return false;
    }

    // Initialize CartridgeLoader
    try {
        cartridgeLoader = std::make_unique<CartridgeLoader>();
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing CartridgeLoader: " << e.what() << std::endl;
        return false;
    }

    try {
        scriptingManager = std::make_unique<ScriptingManager>(this);
        
        // Determine what to load based on cartridgePath
        bool scriptLoaded = false;
        
        if (cartridgePath.empty()) {
            // No cartridge specified - load system menu
            std::cout << "No cartridge specified. Loading system menu..." << std::endl;
            SetState(EngineState::MENU);
            scriptLoaded = scriptingManager->LoadAndRunScript(SystemScripts::MENU_SCRIPT.c_str());
            if (!scriptLoaded) {
                std::cerr << "Could not load the system menu." << std::endl;
                std::cerr << "Error: " << scriptingManager->GetLastLuaError() << std::endl;
                return false;
            }
        } else {
            // Cartridge path specified - try to load it via LoadCartridge
            std::cout << "Loading cartridge via Engine::LoadCartridge(): " << cartridgePath << std::endl;
            
            if (!LoadCartridge(cartridgePath)) {
                std::cerr << "Failed to load cartridge: " << cartridgePath << std::endl;
                return false;
            }
            
            scriptLoaded = true;  // LoadCartridge handles script loading
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error initializing ScriptingManager: " << e.what() << std::endl;
        return false;
    }

    try {
        activeGame = std::make_unique<LuaGame>(scriptingManager.get());
    } catch (const std::exception& e) {
        std::cerr << "Error creating game instance: " << e.what() << std::endl;
        return false;
    }

    isRunning = true;
    startTime = std::chrono::high_resolution_clock::now();
    
    // Only set RUNNING_CARTRIDGE if we're not already in a specific state
    // (MENU state or RUNNING_CARTRIDGE from LoadCartridge should be preserved)
    if (currentState == EngineState::BOOT) {
        SetState(EngineState::RUNNING_CARTRIDGE);
    }
    
    std::cout << "Engine initialized successfully." << std::endl;
    return true;
}

double Engine::getElapsedTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - startTime;
    return elapsed.count();
}

void Engine::Run() {
    using clock = std::chrono::high_resolution_clock;

    SDL_Event event;
    auto previousTime = clock::now();
    double lag = 0.0;

    while (isRunning) {
        auto currentTime = clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(currentTime - previousTime).count();
        previousTime = currentTime;
        lag += elapsed;

        // Prepare for new input.
        inputManager->beginNewFrame();

        // Process all pending events.
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            
            // Handle mouse events (Phase 5.16)
            if (event.type == SDL_MOUSEMOTION || 
                event.type == SDL_MOUSEBUTTONDOWN || 
                event.type == SDL_MOUSEBUTTONUP) {
                inputManager->handleMouseEvent(event);
            }
            
            // Handle gamepad events (Phase 5.18)
            if (event.type == SDL_CONTROLLERDEVICEADDED ||
                event.type == SDL_CONTROLLERDEVICEREMOVED ||
                event.type == SDL_CONTROLLERBUTTONDOWN ||
                event.type == SDL_CONTROLLERBUTTONUP) {
                inputManager->handleGamepadEvent(event);
            }
            
            // The InputManager doesn't need to handle keyboard events directly,
            // as SDL_GetKeyboardState is updated by SDL_PollEvent/SDL_PumpEvents.
        }

        // Fixed timestep logic update loop.
        // It runs as many times as necessary to 'catch up' with real time.
        while (lag >= MS_PER_UPDATE) {
            // Do not update logic if we are in an error state.
            if (activeGame && !inErrorState) {
                if (!activeGame->_update()) {
                    enterErrorState(scriptingManager->GetLastLuaError());
                }
            }
            lag -= MS_PER_UPDATE;
        }

        // Render loop. It runs once per main loop iteration.
        if (inErrorState) {
            drawErrorScreen();
        } else {
            if (activeGame) {
                activeGame->_draw(*aestheticLayer);
            }
        }
        aestheticLayer->Present(); // Present the result regardless.
    }
}


// === Engine State Machine Implementation ===

void Engine::SetState(EngineState newState) {
    if (currentState == newState) {
        return; // No change needed
    }

    previousState = currentState;
    currentState = newState;

    std::cout << "Engine state transition: ";
    
    // Helper lambda to convert state to string for logging
    auto stateToString = [](EngineState state) -> const char* {
        switch (state) {
            case EngineState::BOOT: return "BOOT";
            case EngineState::MENU: return "MENU";
            case EngineState::LOADING_CARTRIDGE: return "LOADING_CARTRIDGE";
            case EngineState::RUNNING_CARTRIDGE: return "RUNNING_CARTRIDGE";
            case EngineState::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    };

    std::cout << stateToString(previousState) << " -> " << stateToString(currentState) << std::endl;

    // Handle state-specific logic
    if (newState == EngineState::ERROR) {
        inErrorState = true;
    } else {
        inErrorState = false;
    }
}

// === Cartridge Lifecycle Management Implementation ===

bool Engine::LoadCartridge(const std::string& cartridgePath) {
    std::cout << "Engine: Loading cartridge from: " << cartridgePath << std::endl;
    
    SetState(EngineState::LOADING_CARTRIDGE);

    // Unload any existing cartridge first
    UnloadCartridge();

    if (!cartridgeLoader) {
        enterErrorState("CartridgeLoader not initialized");
        SetState(EngineState::ERROR);
        return false;
    }

    // Validate cartridge
    if (!cartridgeLoader->IsValidCartridge(cartridgePath)) {
        std::string error = "Invalid cartridge: " + cartridgePath;
        error += "\nError: " + cartridgeLoader->GetLastError();
        enterErrorState(error);
        SetState(EngineState::ERROR);
        return false;
    }

    // Load and parse config.json (returns defaults if no config)
    CartridgeConfig config = cartridgeLoader->LoadCartridge(cartridgePath);
    
    std::cout << "Engine: Cartridge configuration loaded:" << std::endl;
    std::cout << "  - Name: " << config.name << std::endl;
    std::cout << "  - Author: " << config.author << std::endl;
    std::cout << "  - Memory Limit: " << config.memory_limit_mb << " MB" << std::endl;
    std::cout << "  - Code Limit: " << config.lua_code_limit_lines << " lines" << std::endl;
    std::cout << "  - Palette Size: " << config.palette_size << " colors" << std::endl;

    // Apply configuration to engine subsystems
    // 1. Apply palette size to AestheticLayer
    if (aestheticLayer) {
        try {
            aestheticLayer->SetPaletteSize(config.palette_size);
            std::cout << "Engine: Applied palette size: " << config.palette_size << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not set palette size: " << e.what() << std::endl;
            // Not fatal - continue with default palette
        }
    }

    // 2. TODO: Apply framebuffer size (requires AestheticLayer modification)
    
    // Get path to main.lua
    std::string mainLuaPath = cartridgeLoader->GetMainLuaPath(cartridgePath);

    // 3. Count code lines in the script file
    int codeLines = 0;
    {
        std::ifstream file(mainLuaPath);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                codeLines++;
            }
            file.close();
        }
    }
    
    // Load the Lua script
    if (!scriptingManager || !scriptingManager->LoadScriptFromFile(mainLuaPath)) {
        std::string error = "Failed to load cartridge script: " + mainLuaPath;
        if (scriptingManager) {
            error += "\nError: " + scriptingManager->GetLastLuaError();
        }
        enterErrorState(error);
        SetState(EngineState::ERROR);
        return false;
    }

    // 4. Set code line count and log resource stats
    scriptingManager->SetCodeLineCount(codeLines);
    
    std::cout << "\n";
    scriptingManager->LogResourceStats();
    
    // Check limits and warn if approaching
    if (codeLines > config.lua_code_limit_lines) {
        std::cout << "WARNING: Code exceeds configured limit of " 
                  << config.lua_code_limit_lines << " lines!" << std::endl;
    } else if (codeLines > config.lua_code_limit_lines * 0.8) {
        std::cout << "INFO: Code is at " << (codeLines * 100 / config.lua_code_limit_lines) 
                  << "% of configured limit." << std::endl;
    }
    
    float memMB = scriptingManager->GetLuaMemoryUsageMB();
    if (memMB > config.memory_limit_mb) {
        std::cout << "WARNING: Memory exceeds configured limit of " 
                  << config.memory_limit_mb << " MB!" << std::endl;
    }
    std::cout << "\n";

    // Create a new LuaGame instance
    try {
        activeGame = std::make_unique<LuaGame>(scriptingManager.get());
        currentCartridgePath = cartridgePath;
        SetState(EngineState::RUNNING_CARTRIDGE);
        std::cout << "Engine: Cartridge '" << config.name << "' loaded successfully." << std::endl;
        return true;
    } catch (const std::exception& e) {
        enterErrorState(std::string("Error creating game instance: ") + e.what());
        SetState(EngineState::ERROR);
        return false;
    }
}

void Engine::UnloadCartridge() {
    if (currentCartridgePath.empty()) {
        return; // No cartridge loaded
    }

    std::cout << "Engine: Unloading cartridge: " << currentCartridgePath << std::endl;

    // Release the game instance
    activeGame.reset();

    // Clear the script state (we'll need to reinitialize ScriptingManager)
    // For now, we just mark as unloaded
    currentCartridgePath.clear();

    std::cout << "Engine: Cartridge unloaded." << std::endl;
}

bool Engine::ReloadCurrentCartridge() {
    if (currentCartridgePath.empty()) {
        std::cerr << "Engine: Cannot reload - no cartridge is currently loaded." << std::endl;
        return false;
    }

    std::string pathToReload = currentCartridgePath;
    std::cout << "Engine: Reloading cartridge: " << pathToReload << std::endl;
    
    return LoadCartridge(pathToReload);
}

// === Error Handling (Task 4.5.2) ===

void Engine::enterErrorState(const std::string& message) {
    inErrorState = true;
    errorMessage = message;
    SetState(EngineState::ERROR);
    std::cerr << "=== ENGINE ERROR ===" << std::endl;
    std::cerr << errorMessage << std::endl;
    std::cerr << "===================" << std::endl;
}

void Engine::drawErrorScreen() {
    if (!aestheticLayer) return;
    
    // Red background
    aestheticLayer->Clear(8); // Color 8 is red in PICO-8 palette
    
    // Draw error header
    aestheticLayer->Print("ULICS - ERROR", 70, 10, 7);  // White text
    aestheticLayer->Line(0, 25, 255, 25, 7);
    
    // Parse error message into lines (max ~30 chars per line for 256px width)
    std::vector<std::string> lines;
    std::istringstream stream(errorMessage);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Wrap long lines
        while (line.length() > 30) {
            lines.push_back(line.substr(0, 30));
            line = line.substr(30);
        }
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    
    // Display error lines (show up to 20 lines)
    int y = 35;
    int maxLines = std::min(20, static_cast<int>(lines.size()));
    
    for (int i = 0; i < maxLines && y < 220; i++) {
        aestheticLayer->Print(lines[i], 5, y, 6); // Light gray
        y += 10;
    }
    
    // Show controls at bottom
    aestheticLayer->Line(0, 225, 255, 225, 7);
    aestheticLayer->Print("ESC: EXIT  R: RELOAD", 50, 235, 14); // Pink text
    
    aestheticLayer->Present();
    
    // Handle error screen input
    if (inputManager) {
        if (inputManager->isKeyPressed(SDL_SCANCODE_ESCAPE)) {
            isRunning = false;
        }
        if (inputManager->isKeyPressed(SDL_SCANCODE_R)) {
            // Try to reload current cartridge
            inErrorState = false;
            if (!currentCartridgePath.empty()) {
                LoadCartridge(currentCartridgePath);
            }
        }
    }
}

void Engine::Shutdown() {
    cartridgeLoader.reset();
    inputManager.reset();
    scriptingManager.reset();
    aestheticLayer.reset(); // Release the unique_ptr

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    std::cout << "Engine shut down." << std::endl;
}