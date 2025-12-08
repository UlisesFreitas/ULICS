#include "core/Engine.h"
#include "rendering/AestheticLayer.h"
#include "scripting/LuaGame.h" // Include our Lua game bridge.
#include "input/InputManager.h" // Include the new InputManager.
#include <iostream>
#include <chrono>
#include "scripting/ScriptingManager.h"
#include "scripting/EmbeddedScripts.h" // Include our new embedded script header.

Engine::Engine() : isRunning(false), inErrorState(false), errorMessage(""),
                   window(nullptr), renderer(nullptr), aestheticLayer(nullptr), 
                   activeGame(nullptr), scriptingManager(nullptr),
                   inputManager(nullptr) {
    // Constructor
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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

    // Create an accelerated renderer. We disable VSYNC so the game loop is not limited by the monitor's refresh rate.
    // Our fixed timestep will handle consistency.
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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

    try {
        scriptingManager = std::make_unique<ScriptingManager>(aestheticLayer.get(), inputManager.get());
        // Load the demo cartridge directly from the embedded string.
        if (!scriptingManager->LoadAndRunScript(EmbeddedScripts::DEMO_CART)) {
            std::cerr << "Could not load the embedded demo cartridge." << std::endl;
            return false;
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
    std::cout << "Engine initialized successfully." << std::endl;
    return true;
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
            // The InputManager doesn't need to handle events directly for now,
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

void Engine::enterErrorState(const std::string& message) {
    inErrorState = true;
    errorMessage = message;
    std::cerr << "Engine entering error state: " << errorMessage << std::endl;
}

void Engine::drawErrorScreen() {
    aestheticLayer->Clear(2); // Dark Purple background for errors
    aestheticLayer->Print("LUA ERROR:", 4, 4, 8); // Red title
    aestheticLayer->Print(errorMessage, 4, 20, 7); // White error message
}

void Engine::Shutdown() {
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