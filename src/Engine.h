#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize();
    void Run();

private:
    void Shutdown();

    bool isRunning;
    SDL_Window* window;
};

#endif // ENGINE_H