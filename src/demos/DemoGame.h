#ifndef DEMO_GAME_H
#define DEMO_GAME_H

#include "game/Game.h"
#include <cstdint>

class DemoGame : public Game {
public:
    DemoGame();
    virtual ~DemoGame() override = default;

    bool _update() override;
    void _draw(AestheticLayer& aestheticLayer) override;

private:
    int frameCount;
};

#endif // DEMO_GAME_H