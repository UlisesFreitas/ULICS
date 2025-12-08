#ifndef GAME_H
#define GAME_H

class AestheticLayer;

/**
 * @class Game
 * @brief Interfaz base para cualquier juego o cartucho que se ejecute en el motor.
 *
 * Define la estructura fundamental de un juego con funciones para inicialización,
 * actualización de la lógica y dibujo.
 */
class Game {
public:
    virtual ~Game() = default;
    virtual void _update() = 0;
    virtual void _draw(AestheticLayer& aestheticLayer) = 0;
};

#endif // GAME_H