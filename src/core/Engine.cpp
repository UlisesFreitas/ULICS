#include "core/Engine.h"
#include "rendering/AestheticLayer.h"
#include "demos/DemoGame.h" // Incluimos nuestro juego de demostración
#include <iostream>

Engine::Engine() : isRunning(false), window(nullptr), renderer(nullptr),
                   aestheticLayer(nullptr), activeGame(nullptr) {
    // Constructor
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
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
        std::cerr << "Error al crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Error al crear el renderer: " << SDL_GetError() << std::endl;
        Shutdown();
        return false;
    }
    // Mantiene la relación de aspecto al escalar.
    SDL_RenderSetLogicalSize(renderer, AestheticLayer::FRAMEBUFFER_WIDTH, AestheticLayer::FRAMEBUFFER_HEIGHT);

    try {
        aestheticLayer = std::make_unique<AestheticLayer>(renderer);
    } catch (const std::exception& e) {
        std::cerr << "Error al inicializar la capa estética: " << e.what() << std::endl;
        return false;
    }

    try {
        activeGame = std::make_unique<DemoGame>();
    } catch (const std::exception& e) {
        std::cerr << "Error al crear la instancia del juego: " << e.what() << std::endl;
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

        // --- Bucle principal del motor ---
        if (activeGame && aestheticLayer) {
            // 1. Actualizar el estado del juego.
            activeGame->_update();

            // 2. Dibujar el estado actual del juego.
            activeGame->_draw(*aestheticLayer);
        }

        // --- Renderizado ---
        // 2. Presentar el resultado de la capa estética en la pantalla.
        aestheticLayer->Present();
    }
}

void Engine::Shutdown() {
    aestheticLayer.reset(); // Libera el unique_ptr

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    std::cout << "Engine detenido." << std::endl;
}