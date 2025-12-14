void Engine::Run() {
    using clock = std::chrono::high_resolution_clock;

    SDL_Event event;
    auto previousTime = clock::now();

    while (isRunning) {
        auto frameStart = clock::now();
        
        // STEP 1: Capture previous frame's input state
        inputManager->beginNewFrame();

        // STEP 2: Process events (updates current input state)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            
            // Toggle Debug Console with F1 (v1.5.2)
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F1) {
                if (debugConsole) {
                    debugConsole->Toggle();
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
        }

        // STEP 3: Check for file changes (Hot Reload)
        if (hotReload && hotReload->IsEnabled() && currentState == EngineState::RUNNING_CARTRIDGE) {
            if (hotReload->CheckForChanges()) {
                std::cout << "\n=== Hot Reload Triggered ===\n" << std::endl;
                ReloadCurrentCartridge();
            }
        }

        // STEP 4: Update game logic (simple 60fps, no fixed timestep)
        if (activeGame && !inErrorState) {
            if (!activeGame->_update()) {
                enterErrorState(scriptingManager->Get LastLuaError());
            }
        }

        // STEP 5: Render
        if (inErrorState) {
            drawErrorScreen();
        } else {
            if (activeGame) {
                activeGame->_draw(*aestheticLayer);
            }
        }
        
        // Draw debug console on top
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
