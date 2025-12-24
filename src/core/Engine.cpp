#include "core/Engine.h"
#include "core/HotReload.h"  // Hot reload system (v1.5.1)
#include "core/Bootstrap.h"  // Bootstrap system
#include "core/Settings.h"   // Settings manager
#include "ui/DebugConsole.h"  // Debug overlay (v1.5.2)
#include "ui/UISystem.h"      // Custom UI system (Phase 2.0.1)
#include "ui/CodeEditor.h"    // Code editor (Phase 2.0.2-2.0.4)
#include "ui/SpriteEditor.h"  // Sprite editor (Phase 3)
#include "ui/SystemSprites.h" // System UI icons
#include "ui/MenuSystem.h"     // Menu system
#include "capture/Screenshot.h"  // Screenshot system (v1.5.3)
#include "capture/GifRecorder.h"  // GIF recording system (v1.5.4)
#include "audio/AudioManager.h"  // Audio system (Phase 5.12 + Bug 1.1.3 fix)
#include "rendering/AestheticLayer.h"
#include "rendering/Map.h"  // For global map instance
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
#include <filesystem>  // For std::filesystem (Task 3.13 - spritesheet loading)

Engine::Engine() : isRunning(false), inErrorState(false), errorMessage(""),
                   window(nullptr), renderer(nullptr), aestheticLayer(nullptr), 
                   activeGame(nullptr), scriptingManager(nullptr),
                   inputManager(nullptr), cartridgeLoader(nullptr), currentMap(nullptr), hotReload(nullptr), debugConsole(nullptr), gifRecorder(nullptr),
                   audioManager(nullptr),
                   uiSystem(nullptr), codeEditor(nullptr), spriteEditor(nullptr), systemSprites(nullptr),
                   currentState(EngineState::BOOT), previousState(EngineState::BOOT),
                   currentMode(EngineMode::GAME),
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
        // Set color 0 (Black) as transparent by default, like PICO-8
        aestheticLayer->SetTransparentColor(0);
        std::cout << "AestheticLayer initialized (color 0 = transparent)" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing AestheticLayer: " << e.what() << std::endl;
        return false;
    }

    try {
        inputManager = std::make_unique<InputManager>();
        // NOTE: No need to configure mouse scaling!
        // SDL_RenderSetLogicalSize already converts mouse coordinates
        // from window size (800x800) to logical size (256x256) automatically
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing InputManager: " << e.what() << std::endl;
        return false;
    }

    // Enable SDL text input (Phase 2.0.2 - Required for Code Editor)
    SDL_StartTextInput();
    std::cout << "Text input enabled for Code Editor" << std::endl;

    // Initialize HotReload (v1.5.1)
    try {
        hotReload = std::make_unique<HotReload>();
        std::cout << "HotReload enabled - edit files and see changes instantly!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: HotReload failed to initialize: " << e.what() << std::endl;
        // Continue without hot reload
    }
    
    // Initialize Bootstrap system - ensures boot files exist in AppData
    try {
        Bootstrap::Initialize();
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: Bootstrap failed: " << e.what() << std::endl;
        // Continue without bootstrap
    }

    // Initialize DebugConsole (v1.5.2)
    try {
        debugConsole = std::make_unique<DebugConsole>();
        std::cout << "Debug Console ready - press F1 to toggle" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: DebugConsole failed to initialize: " << e.what() << std::endl;
        // Continue without debug console
    }
    
    // Initialize GifRecorder (v1.5.4)
    try {
        gifRecorder = std::make_unique<GifRecorder>();
        std::cout << "GIF Recorder ready - hold Ctrl+F12 to record 5 seconds" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: GifRecorder failed to initialize: " << e.what() << std::endl;
        // Continue without GIF recording
    }

    // Initialize Audio System (Phase 5.12 + Bug 1.1.3 fix)
    try {
        audioManager = &AudioManager::getInstance();
        if (audioManager->Initialize()) {
            std::cout << "Audio system initialized with lock-free ring buffer" << std::endl;
        } else {
            std::cerr << "Warning: Audio failed to initialize" << std::endl;
            audioManager = nullptr;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: AudioManager failed to initialize: " << e.what() << std::endl;
        audioManager = nullptr;
        // Continue without audio
    }

    // Initialize UI System (Phase 2.0.1)
    try {
        uiSystem = std::make_unique<UISystem>();
        std::cout << "UI System initialized" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: UISystem failed to initialize: " << e.what() << std::endl;
        // Continue without UI System
    }

    // Initialize System Sprites (icon system) - MUST BE EARLY
    try {
        systemSprites = std::make_unique<SystemSprites>();
        systemSprites->Initialize();  // Loads or generates system_sprites.png
        std::cout << "System sprites initialized" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: SystemSprites failed to initialize: " << e.what() << std::endl;
        // Continue without system sprites
    }

    // Initialize Code Editor (Phase 2.0.2-2.0.4)
    try {
        codeEditor = std::make_unique<CodeEditor>();
        std::cout << "Code Editor ready - press F1 to toggle" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: CodeEditor failed to initialize: " << e.what() << std::endl;
        // Continue without Code Editor
    }
    
    // Initialize Pause Menu
    try {
        pauseMenu = std::make_unique<MenuSystem>();
        pauseMenu->SetTitle("PAUSED");
        pauseMenu->SetVisible(false);  // Hidden by default
        
        // Add menu options
        pauseMenu->AddItem("RESUME", [this]() {
            SetState(EngineState::RUNNING_CARTRIDGE);
            pauseMenu->SetVisible(false);
        });
        
        pauseMenu->AddItem("RESTART", [this]() {
            ReloadCurrentCartridge();
            SetState(EngineState::RUNNING_CARTRIDGE);
            pauseMenu->SetVisible(false);
        });
        
        pauseMenu->AddItem("SETTINGS", [this]() {
            pauseMenu->SetVisible(false);
            SetState(EngineState::SETTINGS_MENU);
            if (settingsMenu) {
                settingsMenu->SetVisible(true);
            }
        });
        
        pauseMenu->AddItem("QUIT TO MENU", [this]() {
            UnloadCartridge();
            SetState(EngineState::MAIN_MENU);
            pauseMenu->SetVisible(false);
            
            // Reset palette to default before loading menu
            if (aestheticLayer) {
                aestheticLayer->ResetToDefaultPalette();
            }
            
            // Load menu script
            if (scriptingManager) {
                bool scriptLoaded = scriptingManager->LoadAndRunScript(SystemScripts::MENU_SCRIPT.c_str());
                if (scriptLoaded) {
                    try {
                        activeGame = std::make_unique<LuaGame>(scriptingManager.get());
                        std::cout << "[Engine] Menu loaded successfully" << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "[Engine] Failed to create menu game: " << e.what() << std::endl;
                    }
                }
            }
        });
        
        std::cout << "Pause Menu initialized" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: PauseMenu failed to initialize: " << e.what() << std::endl;
        // Continue without Pause Menu
    }
    
    // Initialize Settings
    try {
        settings = std::make_unique<Settings>();
        settings->Load();  // Load from AppData or use defaults
        std::cout << "Settings loaded" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: Settings failed to initialize: " << e.what() << std::endl;
        // Continue with default settings
    }
    
    // Initialize Settings Menu
    try {
        settingsMenu = std::make_unique<MenuSystem>();
        settingsMenu->SetTitle("SETTINGS");
        settingsMenu->SetVisible(false);
        
        if (settings) {
            // Add settings controls
            settingsMenu->AddSlider("VOLUME", settings->GetVolumePtr(), 0, 100, "%");
            settingsMenu->AddToggle("FULLSCREEN", settings->GetFullscreenPtr());
            settingsMenu->AddToggle("DEBUG MODE", settings->GetDebugModePtr());
            settingsMenu->AddToggle("VSYNC", settings->GetVSyncPtr());
            
            settingsMenu->AddSeparator();
            
            settingsMenu->AddItem("RESET TO DEFAULTS", [this]() {
                if (settings) {
                    settings->ResetToDefaults();
                    settings->Save();
                    std::cout << "[Settings] Reset to defaults" << std::endl;
                }
            });
            
            settingsMenu->AddItem("BACK", [this]() {
                // Auto-save settings on back
                if (settings) {
                    settings->Save();
                }
                settingsMenu->SetVisible(false);
                // Return to pause menu
                if (currentState == EngineState::SETTINGS_MENU) {
                    SetState(EngineState::PAUSE_MENU);
                    pauseMenu->SetVisible(true);
                }
            });
        }
        
        std::cout << "Settings Menu initialized" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: SettingsMenu failed to initialize: " << e.what() << std::endl;
        // Continue without Settings Menu
    }

    // Initialize Sprite Editor (Phase 3)
    try {
        spriteEditor = std::make_unique<SpriteEditor>();
        // Connect system sprites to sprite editor
        if (systemSprites) {
            spriteEditor->SetSystemSprites(systemSprites.get());
        }
        std::cout << "Sprite Editor ready - press F2 to toggle" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Warning: SpriteEditor failed to initialize: " << e.what() << std::endl;
        // Continue without Sprite Editor
    }

    // Initialize CartridgeLoader
    try {
        cartridgeLoader = std::make_unique<CartridgeLoader>();
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing CartridgeLoader: " << e.what() << std::endl;
        return false;
    }

    // Create a default map instance (v1.1 - Task 1.1.4)
    try {
        currentMap = std::make_unique<Map>();
        std::cout << "Default map created (128x64)" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating map: " << e.what() << std::endl;
        // Continue anyway - map functions will just not work
    }

    try {
        scriptingManager = std::make_unique<ScriptingManager>(this);
        
        // Determine what to load based on cartridgePath
        bool scriptLoaded = false;
        
        if (cartridgePath.empty()) {
            // No cartridge specified - load system menu
            std::cout << "No cartridge specified. Loading system menu..." << std::endl;
            SetState(EngineState::MAIN_MENU);
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
        auto frameStart = clock::now();
        
        // STEP 1: Capture previous frame's input state
        inputManager->beginNewFrame();

        // STEP 2: Process events (updates current input state)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            
            // Handle file drop (for Sprite Editor import)
            if (event.type == SDL_DROPFILE) {
                char* droppedFile = event.drop.file;
                std::cout << "[Engine] File dropped: " << droppedFile << std::endl;
                
                // Pass to Sprite Editor if active
                if (spriteEditor && spriteEditor->IsActive()) {
                    spriteEditor->OnFileDropped(droppedFile);
                }
                
                SDL_free(droppedFile);
            }
            
            // Toggle Debug Console with Ctrl+Alt+1 (v1.5.2 - Updated 2025-12-14)
            // Changed from F1 to free up F-keys for editors
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_1) {
                const Uint8* keystate = SDL_GetKeyboardState(NULL);
                bool ctrlPressed = keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_RCTRL];
                bool altPressed = keystate[SDL_SCANCODE_LALT] || keystate[SDL_SCANCODE_RALT];
                
                if (ctrlPressed && altPressed) {
                    if (debugConsole) {
                        debugConsole->Toggle();
                    }
                }
            }
            
            // Mode Switching (Phase 2.0.5 + Phase 3)
            // F1 - Code Editor
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F1) {
                std::ofstream log("sprite_editor_log.txt", std::ios::app);
                if (log.is_open()) {
                    log << "[Engine] F1 pressed, current mode: " << static_cast<int>(currentMode) << std::endl;
                    log.close();
                }
                if (currentMode == EngineMode::CODE_EDITOR) {
                    SetMode(EngineMode::GAME);
                } else {
                    SetMode(EngineMode::CODE_EDITOR);
                }
            }
            
            // F2 - Sprite Editor
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F2) {
                std::ofstream log("sprite_editor_log.txt", std::ios::app);
                if (log.is_open()) {
                    log << "[Engine] F2 pressed, current mode: " << static_cast<int>(currentMode) << std::endl;
                    log.close();
                }
                if (currentMode == EngineMode::SPRITE_EDITOR) {
                    SetMode(EngineMode::GAME);
                } else {
                    SetMode(EngineMode::SPRITE_EDITOR);
                }
            }
            
            // ESC - Return to game
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                if (currentMode != EngineMode::GAME) {
                    SetMode(EngineMode::GAME);
                }
            }
            
            // Take Screenshot with F12 (v1.5.3)
            // Start GIF Recording with Ctrl+F12 (v1.5.4)
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F12) {
                SDL_Keymod modState = SDL_GetModState();
                
                if (modState & KMOD_CTRL) {
                    // Ctrl+F12 - GIF Recording
                    if (gifRecorder && aestheticLayer) {
                        if (!gifRecorder->IsRecording()) {
                            gifRecorder->StartRecording(256, 256);  // ULICS screen size
                        }
                    }
                } else {
                    // F12 alone - Screenshot
                    if (aestheticLayer) {
                        aestheticLayer->CaptureScreenshot();
                    }
                }
            }
            
            // Handle mouse events (Phase 5.16 + 2.0.5.3)
            if (event.type == SDL_MOUSEMOTION || 
                event.type == SDL_MOUSEBUTTONDOWN || 
                event.type == SDL_MOUSEBUTTONUP ||
                event.type == SDL_MOUSEWHEEL) {
                inputManager->handleMouseEvent(event);
            }
            
            // Handle gamepad events (Phase 5.18)
            if (event.type == SDL_CONTROLLERDEVICEADDED ||
                event.type == SDL_CONTROLLERDEVICEREMOVED ||
                event.type == SDL_CONTROLLERBUTTONDOWN ||
                event.type == SDL_CONTROLLERBUTTONUP) {
                inputManager->handleGamepadEvent(event);
            }

            // Handle text input events (Phase 2.0.2 - Code Editor)
            if (event.type == SDL_TEXTINPUT) {
                inputManager->handleTextInput(event);
            }
        }

        // STEP 3: Check for file changes (Hot Reload)
        if (hotReload && hotReload->IsEnabled() && currentState == EngineState::RUNNING_CARTRIDGE) {
            if (hotReload->CheckForChanges()) {
                std::cout << "\n=== Hot Reload Triggered ===\n" << std::endl;
                ReloadCurrentCartridge();
            }
        }

        //  STEP 4: Update game logic OR editor OR pause menu (Phase 2.0.5)
        if (currentState == EngineState::PAUSE_MENU) {
            // Pause menu active - update menu
            if (pauseMenu && inputManager) {
                bool menuActive = pauseMenu->Update(*inputManager);
                if (!menuActive) {
                    // ESC pressed in menu - resume game
                    SetState(EngineState::RUNNING_CARTRIDGE);
                    pauseMenu->SetVisible(false);
                }
            }
        } else if (currentState == EngineState::SETTINGS_MENU) {
            // Settings menu active - update menu
            if (settingsMenu && inputManager) {
                bool menuActive = settingsMenu->Update(*inputManager);
                if (!menuActive) {
                    // ESC pressed in settings - auto-save and back to pause menu
                    if (settings) {
                        settings->Save();
                    }
                    SetState(EngineState::PAUSE_MENU);
                    settingsMenu->SetVisible(false);
                    if (pauseMenu) {
                        pauseMenu->SetVisible(true);
                    }
                }
            }
        } else if (currentMode == EngineMode::GAME) {
            // Check for ESC to pause
            if (inputManager->isKeyPressed(SDL_SCANCODE_ESCAPE) && 
                currentState == EngineState::RUNNING_CARTRIDGE) {
                std::cout << "[Engine] ESC pressed - pausing game" << std::endl;
                SetState(EngineState::PAUSE_MENU);
                if (pauseMenu) {
                    pauseMenu->SetVisible(true);
                }
            }
            
            // Game mode - run normal update (only if not paused)
            if (activeGame && !inErrorState && currentState != EngineState::PAUSE_MENU) {
                if (!activeGame->_update()) {
                    enterErrorState(scriptingManager->GetLastLuaError());
                }
            }
        } else if (currentMode == EngineMode::CODE_EDITOR) {
            // Code Editor mode - update editor
            if (codeEditor && inputManager) {
                codeEditor->Update(*inputManager);
            }
        } else if (currentMode == EngineMode::SPRITE_EDITOR) {
            // Sprite Editor mode - update editor (Phase 3)
            if (spriteEditor && inputManager) {
                static int frameCount = 0;
                if (frameCount++ % 60 == 0) {  // Log once per second
                    std::ofstream log("sprite_editor_log.txt", std::ios::app);
                    if (log.is_open()) {
                        log << "[Engine] Calling SpriteEditor::Update() frame: " << frameCount << std::endl;
                        log.close();
                    }
                }
                spriteEditor->Update(*inputManager);
            }
        }
        // TODO: Add handlers for other editor modes (SPRITE, MAP, SFX, MUSIC)

        // STEP 4.5: Generate audio for this frame (Bug 1.1.3 fix)
        // This runs in main thread and writes to ring buffer
        // Audio callback reads from ring buffer in separate thread (lock-free!)
        if (audioManager && audioManager->IsInitialized()) {
            // Generate enough samples for this frame (~735 samples at 44.1kHz/60fps)
            int samplesPerFrame = audioManager->GetSampleRate() / 60;
            audioManager->GenerateAudio(samplesPerFrame);
        }

        // STEP 5: Render (Phase 2.0.5)
        if (inErrorState) {
            drawErrorScreen();
        } else if (currentState == EngineState::PAUSE_MENU) {
            // Pause menu - render game frozen in background
            if (activeGame) {
                activeGame->_draw(*aestheticLayer);
            }
            // Render pause menu on top
            if (pauseMenu) {
                pauseMenu->Render(*aestheticLayer);
            }
        } else if (currentState == EngineState::SETTINGS_MENU) {
            // Settings menu - render game frozen in background
            if (activeGame) {
                activeGame->_draw(*aestheticLayer);
            }
            // Render settings menu on top
            if (settingsMenu) {
                settingsMenu->Render(*aestheticLayer);
            }
        } else if (currentMode == EngineMode::GAME) {
            // Game mode - render game
            if (activeGame) {
                activeGame->_draw(*aestheticLayer);
            }
        } else if (currentMode == EngineMode::CODE_EDITOR) {
            // Code Editor mode - render editor
            if (codeEditor && uiSystem) {
                codeEditor->Render(*aestheticLayer, *uiSystem);
            }
        } else if (currentMode == EngineMode::SPRITE_EDITOR) {
            // Sprite Editor mode - render editor (Phase 3)
            if (spriteEditor) {
                spriteEditor->Render(*aestheticLayer, *inputManager);
            }
        }
        // TODO: Add render handlers for other editor modes
        
        // Draw debug console on top (works in any mode)
        if (debugConsole) {
            auto currentTime = clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(currentTime - previousTime).count();
            previousTime = currentTime;
            debugConsole->UpdateFPS(elapsed);
            debugConsole->Draw(*aestheticLayer);
        }
        
        aestheticLayer->Present();
        
        // Feed frame to GIF recorder if recording
        if (gifRecorder && gifRecorder->IsRecording()) {
            gifRecorder->AddFrame(aestheticLayer->GetPixelData());
        }
        
        // STEP 6: Cap to ~60 FPS
        auto frameEnd = clock::now();
        auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
        auto targetFrameTime = std::chrono::milliseconds(16); // ~60 FPS
        
        if (frameDuration < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameDuration);
        }
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
            case EngineState::MAIN_MENU: return "MAIN_MENU";
            case EngineState::PAUSE_MENU: return "PAUSE_MENU";
            case EngineState::SETTINGS_MENU: return "SETTINGS_MENU";
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
            
            // Load custom palette.pal if it exists (RGB colors for sprites)
            std::filesystem::path cartPath(cartridgePath);
            std::filesystem::path palettePath = cartPath / "palette.pal";
            
            if (std::filesystem::exists(palettePath)) {
                std::cout << "Engine: Found palette.pal, loading..." << std::endl;
                
                std::ifstream paletteFile(palettePath, std::ios::binary);
                if (paletteFile.is_open()) {
                    std::vector<SDL_Color> palette;
                    for (int i = 0; i < 32; i++) {
                        uint8_t r, g, b;
                        paletteFile.read(reinterpret_cast<char*>(&r), 1);
                        paletteFile.read(reinterpret_cast<char*>(&g), 1);
                        paletteFile.read(reinterpret_cast<char*>(&b), 1);
                        
                        if (!paletteFile.fail()) {
                            palette.push_back({r, g, b, 255});
                        }
                    }
                    paletteFile.close();
                    
                    if (palette.size() == 32) {
                        aestheticLayer->LoadPalette(palette);
                        std::cout << "Engine: Loaded custom palette (32 colors)" << std::endl;
                    } else {
                        std::cout << "Engine: Invalid palette.pal, using default" << std::endl;
                    }
                }
            } else {
                std::cout << "Engine: No custom palette.pal found, using default" << std::endl;
            }
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
    
    // Start watching main.lua for changes (v1.5.1 - Hot Reload)
    if (hotReload) {
        hotReload->StopWatching();  // Clear previous watches
        hotReload->WatchFile(mainLuaPath);
    }
    
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
        
        // Load spritesheet (Task 3.13 - Sprite Editor integration)
        if (aestheticLayer) {
            std::filesystem::path cartPath(cartridgePath);
            std::filesystem::path spritesheetPath = cartPath / "spritesheet.png";
            
            if (std::filesystem::exists(spritesheetPath)) {
                std::cout << "Engine: Loading sprite sheet from cartridge..." << std::endl;
                if (aestheticLayer->LoadSpriteSheet(spritesheetPath.string())) {
                    std::cout << "Engine: Sprite sheet loaded successfully" << std::endl;
                } else {
                    std::cout << "Engine: Warning - failed to load sprite sheet" << std::endl;
                }
            } else {
                std::cout << "Engine: No spritesheet.png found in cartridge" << std::endl;
            }
            
            // Load palette.pal from cartridge (Task 3.14 fix)
            std::filesystem::path palettePath = cartPath / "palette.pal";
            if (std::filesystem::exists(palettePath)) {
                std::cout << "Engine: Loading palette.pal from cartridge..." << std::endl;
                
                // Read palette file (32 colors * 3 bytes RGB = 96 bytes)
                std::ifstream paletteFile(palettePath, std::ios::binary);
                if (paletteFile.is_open()) {
                    std::vector<SDL_Color> cartPalette;
                    for (int i = 0; i < 32; i++) {
                        uint8_t r, g, b;
                        paletteFile.read(reinterpret_cast<char*>(&r), 1);
                        paletteFile.read(reinterpret_cast<char*>(&g), 1);
                        paletteFile.read(reinterpret_cast<char*>(&b), 1);
                        
                        if (!paletteFile.fail()) {
                            cartPalette.push_back({r, g, b, 255});
                        }
                    }
                    paletteFile.close();
                    
                    if (cartPalette.size() == 32) {
                        aestheticLayer->LoadPalette(cartPalette);
                        std::cout << "Engine: Palette loaded successfully (32 colors)" << std::endl;
                    }
                }
            } else {
                std::cout << "Engine: No palette.pal found, using default palette" << std::endl;
            }
        }
        
        // Load sprite flags (Task 3.15 - Sprite flags for collision detection)
        if (spriteEditor) {
            std::filesystem::path cartPath(cartridgePath);
            std::filesystem::path flagsPath = cartPath / "spritesheet.flags";
            
            if (std::filesystem::exists(flagsPath)) {
                std::cout << "Engine: Loading sprite flags from cartridge..." << std::endl;
                
                // Tell SpriteEditor to load flags from this path
                spriteEditor->SetCartridgePath(cartPath.string());
                spriteEditor->LoadSpriteFlags();
                
                std::cout << "Engine: Sprite flags loaded successfully" << std::endl;
            } else {
                std::cout << "Engine: No spritesheet.flags found in cartridge" << std::endl;
            }
        }
        
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

// === Mode Switching (Phase 2.0.5) ===

void Engine::SetMode(EngineMode newMode) {
    currentMode = newMode;
    
    // Initialize Code Editor with current cartridge when entering CODE_EDITOR mode
    if (currentMode == EngineMode::CODE_EDITOR && codeEditor && !currentCartridgePath.empty()) {
        std::string mainLuaPath = currentCartridgePath + "/main.lua";
        codeEditor->Initialize(mainLuaPath);
    }
    
    // Initialize Sprite Editor when entering SPRITE_EDITOR mode (Phase 3)
    if (currentMode == EngineMode::SPRITE_EDITOR && spriteEditor && !currentCartridgePath.empty()) {
        std::ofstream log("sprite_editor_log.txt", std::ios::app);
        if (log.is_open()) {
            log << "[Engine::SetMode] Activating Sprite Editor" << std::endl;
            log.close();
        }
        std::string spritesheetPath = currentCartridgePath + "/spritesheet.png";
        
        // Pass aestheticLayer for palette loading (Task 3.14)
        spriteEditor->Initialize(spritesheetPath, aestheticLayer.get());
        spriteEditor->SetActive(true);
        
        std::ofstream log2("sprite_editor_log.txt", std::ios::app);
        if (log2.is_open()) {
            log2 << "[Engine::SetMode] Sprite Editor activated" << std::endl;
            log2.close();
        }
    } else if (spriteEditor) {
        std::ofstream log("sprite_editor_log.txt", std::ios::app);
        if (log.is_open()) {
            log << "[Engine::SetMode] Deactivating Sprite Editor" << std::endl;
            log.close();
        }
        spriteEditor->SetActive(false);
        
        // Reload spritesheet after editing (Task 3.13 - Hot reload)
        if (currentMode == EngineMode::GAME && aestheticLayer && !currentCartridgePath.empty()) {
            std::cout << "Engine: Reloading sprite sheet after editing..." << std::endl;
            if (aestheticLayer->ReloadSpriteSheet()) {
                std::cout << "Engine: Sprite sheet hot-reloaded successfully!" << std::endl;
            }
        }
    }
    
    std::cout << "Mode switched to: ";
    switch (currentMode) {
        case EngineMode::GAME: std::cout << "GAME"; break;
        case EngineMode::CODE_EDITOR: std::cout << "CODE_EDITOR"; break;
        case EngineMode::SPRITE_EDITOR: std::cout << "SPRITE_EDITOR"; break;
        case EngineMode::MAP_EDITOR: std::cout << "MAP_EDITOR (not implemented)"; break;
        case EngineMode::SFX_EDITOR: std::cout << "SFX_EDITOR (not implemented)"; break;
        case EngineMode::MUSIC_EDITOR: std::cout << "MUSIC_EDITOR (not implemented)"; break;
    }
    std::cout << std::endl;
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