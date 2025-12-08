#include "Engine.h"
#include <iostream>

Engine::Engine() : isRunning(false), window(nullptr) {
    // Constructor
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "UliCS Fantasy Console",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Error al crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    isRunning = true;
    std::cout << "Engine inicializado correctamente." << std::endl;
    return true;
}

void Engine::Run() {
    SDL_Event event;
    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        // Aquí iría la lógica de _update() y _draw() en el futuro

        SDL_Delay(16); // Limitar a ~60 FPS
    }
}

void Engine::Shutdown() {
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    std::cout << "Engine detenido." << std::endl;
}