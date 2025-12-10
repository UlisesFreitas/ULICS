#include "core/Engine.h"
#include "rendering/AestheticLayer.h"
#include "scripting/LuaGame.h"
#include "input/InputManager.h"
#include "cartridge/GameLoader.h"
#include "core/Constants.h"
#include "core/FileSystem.h"
#include "cartridge/EmbeddedBootCartridge.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>

// No forward declaration needed, GameLoader.h provides it.

Engine::Engine(bool headless) : isRunning(false),
                   isHeadless(headless),
                   currentState(EngineState::Initializing), window(nullptr), renderer(nullptr) {
    // Constructor
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(const char* title, int width, int height) {
    if (isHeadless) {
        enterErrorState("Cannot call graphical Initialize() on a headless engine.");
        return false;
    }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
        enterErrorState("SDL Init failed: " + std::string(SDL_GetError()));
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        enterErrorState("Window creation failed: " + std::string(SDL_GetError()));
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        enterErrorState("Renderer creation failed: " + std::string(SDL_GetError()));
        return false;
    }
    SDL_RenderSetLogicalSize(renderer, AestheticLayer::FRAMEBUFFER_WIDTH, AestheticLayer::FRAMEBUFFER_HEIGHT);

    // Initialize core subsystems
    aestheticLayer = std::make_unique<AestheticLayer>(renderer);
    inputManager = std::make_unique<InputManager>();
    gameLoader = std::make_unique<GameLoader>(this);

    // Set up user data directory and deploy boot cartridge if needed
    userDataPath = Ulics::FileSystem::getUserDataPath(Ulics::Constants::ORGANIZATION_NAME.data(), Ulics::Constants::APP_NAME.data());
    if (userDataPath.empty()) {
        enterErrorState("Could not determine user data path.");
        return true; // Return true to allow error screen to be shown.
    }
    std::cout << "User data path set to: " << userDataPath << std::endl;
    deployDefaultCartridgeIfNeeded();

    // Synchronously load the boot cartridge on startup. This is the only time we block.
    activeGame = GameLoader::loadAndInitializeGame(this, ".ulics_boot", nullptr);

    if (!activeGame) {
        enterErrorState("Failed to load embedded boot cartridge.");
        return true;
    }
    
    // Apply boot cartridge config
    auto* luaGame = static_cast<LuaGame*>(activeGame.get());
    const auto& config = luaGame->getConfig();
    size_t paletteSize = config.value("/config/palette_size"_json_pointer, 16);
    aestheticLayer->ResizePalette(paletteSize);
    std::cout << "Engine: Boot cartridge palette size set to " << paletteSize << std::endl;

    isRunning = true;
    currentState = EngineState::BootCartridgeRunning;
    startTime = std::chrono::high_resolution_clock::now();
    std::cout << "Engine initialized successfully." << std::endl;
    return true;
}

bool Engine::InitializeHeadless(const std::string& testUserDataPath) {
    if (!isHeadless) {
        enterErrorState("Cannot call InitializeHeadless() on a graphical engine.");
        return false;
    }

    // For headless tests, we don't initialize SDL video, window, or renderer.
    // We only need the subsystems required for loading and logic.
    inputManager = std::make_unique<InputManager>();
    gameLoader = std::make_unique<GameLoader>(this);
    userDataPath = testUserDataPath;

    // No game is loaded, no state is set to running.
    // The engine is now ready for test-driven commands.
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

        // 1. Update the input manager to capture the new frame's state.
        // This internally handles event pumping.
        inputManager->update();

        // 2. Process the event queue, primarily for the quit event.
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                    inputManager->addController(event.cdevice.which);
                    break;
                case SDL_CONTROLLERDEVICEREMOVED:
                    inputManager->removeController(event.cdevice.which);
                    break;
            }
        }

        // Handle states and updates
        switch (currentState) {
            case EngineState::BootCartridgeRunning:
            case EngineState::GameRunning: {
                while (lag >= MS_PER_UPDATE) {
                    if (activeGame && !activeGame->_update()) {
                        // A runtime Lua error occurred.
                        // For now, we don't have a good way to get the error from the background
                        // so we show a generic message.
                        enterErrorState("A runtime error occurred in the cartridge.");
                        break;
                    }
                    lag -= MS_PER_UPDATE;
                }
                if(activeGame) activeGame->_draw(*aestheticLayer);
                break;
            }
            case EngineState::Loading: {
                drawLoadingScreen();

                // Check if the future has a result without blocking.
                if (nextGameFuture.valid() && nextGameFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    std::unique_ptr<LuaGame> newGame = nextGameFuture.get();
                    
                    if (newGame) {
                        // Success! Swap the new game in. This is fast.
                        activeGame = std::move(newGame);

                        // Apply new cartridge config
                        auto* luaGame = static_cast<LuaGame*>(activeGame.get());
                        const auto& config = luaGame->getConfig();
                        size_t paletteSize = config.value("/config/palette_size"_json_pointer, 16);
                        aestheticLayer->ResizePalette(paletteSize);
                        std::cout << "Engine: New cartridge palette size set to " << paletteSize << std::endl;

                        currentState = EngineState::GameRunning;
                        std::cout << "Engine: Async load finished. Switched to running state." << std::endl;
                    } else {
                        // The background loading failed.
                        enterErrorState("Failed to load the requested cartridge.");
                    }
                }
                lag = 0.0; // Prevent lag accumulation while loading.
                break;
            }
            case EngineState::Error: {
                drawErrorScreen();
                lag = 0.0;
                break;
            }
            case EngineState::Initializing: {
                // Should not happen in Run() loop.
                lag = 0.0;
                break;
            }
        }
        
        aestheticLayer->Present();
    }
}

void Engine::enterErrorState(const std::string& message) {
    errorMessage = message;
    currentState = EngineState::Error;
    std::cerr << "Engine entering error state: " << errorMessage << std::endl;
}

void Engine::RequestCartridgeLoad(const std::string& cartId) {
    if (currentState == EngineState::Loading) {
        std::cout << "Engine: Ignoring load request, a cartridge is already being loaded." << std::endl;
        return;
    }
    std::cout << "Engine: Queued request to load cartridge '" << cartId << "'." << std::endl;
    auto loadResult = gameLoader->loadGameAsync(cartId);
    nextGameFuture = std::move(loadResult.gameFuture);
    loadProgress = loadResult.progress;
    currentState = EngineState::Loading;
}

void Engine::deployDefaultCartridgeIfNeeded() {
    std::filesystem::path bootCartridgeDir = std::filesystem::path(userDataPath) / "cartridges" / ".ulics_boot";
    std::filesystem::path bootConfigPath = bootCartridgeDir / "config.json";
    std::filesystem::path bootScriptPath = bootCartridgeDir / "main.lua";

    // Always overwrite the boot cartridge to ensure it's the correct version for system stability.
    std::cout << "Deploying embedded boot cartridge to ensure system integrity..." << std::endl;

    try {
        std::filesystem::create_directories(bootCartridgeDir);

        std::ofstream configFile(bootConfigPath, std::ios::binary | std::ios::trunc);
        configFile << Ulics::EmbeddedCartridge::BOOT_CONFIG_JSON;
        configFile.close();

        std::ofstream scriptFile(bootScriptPath, std::ios::binary | std::ios::trunc);
        scriptFile << Ulics::EmbeddedCartridge::BOOT_LUA_SCRIPT;
        scriptFile.close();

        std::cout << "Boot cartridge deployment successful." << std::endl;

    } catch (const std::filesystem::filesystem_error& e) {
        enterErrorState("Fatal: Failed to write default cartridge files: " + std::string(e.what()));
    }
}

void Engine::drawLoadingScreen() {
    aestheticLayer->SetCamera(0, 0);
    aestheticLayer->Clear(1); // Dark Blue background

    // Draw the progress bar
    float progress = loadProgress ? loadProgress->load() : 0.0f;
    int barWidth = static_cast<int>(200 * progress);
    aestheticLayer->RectFill(28, 118, 200, 10, 0); // Black background for the bar
    aestheticLayer->RectFill(28, 118, barWidth, 10, 7); // White progress

    aestheticLayer->Print("LOADING...", 100, 124, 7); // White text
}

void Engine::drawErrorScreen() {
    aestheticLayer->SetCamera(0, 0);
    aestheticLayer->Clear(2); // Dark Purple background for errors
    aestheticLayer->Print("ENGINE ERROR:", 4, 4, 8); // Red title
    aestheticLayer->Print(errorMessage, 4, 20, 7); // White error message
}

void Engine::Shutdown() {
    // Resetting unique_ptrs will handle deletion.
    activeGame.reset();
    gameLoader.reset();
    inputManager.reset();
    aestheticLayer.reset(); 

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    
    renderer = nullptr;
    window = nullptr;

    SDL_Quit();
    std::cout << "Engine shut down." << std::endl;
}