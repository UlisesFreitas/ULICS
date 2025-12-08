#ifndef GAME_H
#define GAME_H

class AestheticLayer;

/// @class Game
/// @brief Base interface for any game or cartridge running on the engine.
///
/// Defines the fundamental structure of a game with functions for initialization,
/// logic updates, and drawing.
class Game {
public:
    virtual ~Game() = default;
    virtual void _update() = 0;
    virtual void _draw(AestheticLayer& aestheticLayer) = 0;
};

#endif // GAME_H