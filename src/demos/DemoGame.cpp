#include "demos/DemoGame.h"
#include "rendering/AestheticLayer.h"

DemoGame::DemoGame() : frameCount(0) {
    // Constructor: Inicializa el estado del juego de demostración.
}

void DemoGame::_update() {
    frameCount++;
}

void DemoGame::_draw(AestheticLayer& aestheticLayer) {
    // Esta es la lógica de dibujo que antes estaba en Engine::Run().

    // Limpiamos el framebuffer con el color de fondo.
    aestheticLayer.Clear(1);

    // Dibujamos algunas formas para demostrar la API.
    aestheticLayer.RectFill(10, 10, 50, 30, 2); // Rectángulo relleno púrpura
    aestheticLayer.Rect(70, 10, 40, 40, 10);    // Contorno de rectángulo amarillo

    // Dibujamos una línea diagonal que cambia de color.
    uint8_t lineColor = 7 + (frameCount / 30) % 9; // Cambia entre blanco y los colores vivos
    aestheticLayer.Line(10, 50, 245, 200, lineColor);

    // Dibujamos un pequeño rectángulo que se mueve.
    aestheticLayer.RectFill((frameCount * 2) % 246, 220, 10, 10, 8); // Rojo
}