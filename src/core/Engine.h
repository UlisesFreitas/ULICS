#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include <memory>

class AestheticLayer;
class Game;

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(const char* title, int width, int height);
    void Run();

private:
    // Constantes para el bucle de juego con timestep fijo.
    static constexpr int UPDATES_PER_SECOND = 60;
    static constexpr double MS_PER_UPDATE = 1000.0 / UPDATES_PER_SECOND;

    void Shutdown();

    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<AestheticLayer> aestheticLayer;
    std::unique_ptr<Game> activeGame;
};

#endif // ENGINE_H