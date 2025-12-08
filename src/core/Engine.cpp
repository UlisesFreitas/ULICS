#include "core/Engine.h"
#include "rendering/AestheticLayer.h"
#include "demos/DemoGame.h" // Incluimos nuestro juego de demostración
#include <iostream>
#include <chrono>
#include "scripting/ScriptingManager.h"

Engine::Engine() : isRunning(false), window(nullptr), renderer(nullptr),
                   aestheticLayer(nullptr), activeGame(nullptr), scriptingManager(nullptr) {
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

    // Creamos el renderer acelerado. Quitamos VSYNC para que el bucle de juego no se vea limitado por la tasa de refresco del monitor.
    // Nuestro timestep fijo se encargará de la consistencia.
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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

    try {
        scriptingManager = std::make_unique<ScriptingManager>();
    } catch (const std::exception& e) {
        std::cerr << "Error al inicializar el ScriptingManager: " << e.what() << std::endl;
        return false;
    }

    isRunning = true;
    std::cout << "Engine inicializado correctamente." << std::endl;
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

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        // Bucle de actualización de lógica con timestep fijo.
        // Se ejecuta tantas veces como sea necesario para 'ponerse al día' con el tiempo real.
        while (lag >= MS_PER_UPDATE) {
            if (activeGame) {
                activeGame->_update();
            }
            lag -= MS_PER_UPDATE;
        }

        // Bucle de renderizado. Se ejecuta una vez por vuelta del bucle principal.
        if (activeGame && aestheticLayer) {
            activeGame->_draw(*aestheticLayer);
            aestheticLayer->Present();
        }
    }
}

void Engine::Shutdown() {
    scriptingManager.reset();
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