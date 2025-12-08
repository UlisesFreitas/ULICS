#include "demos/DemoGame.h"
#include "rendering/AestheticLayer.h"

DemoGame::DemoGame() : frameCount(0) {
    // Constructor: Initializes the demo game state.
}

void DemoGame::_update() {
    frameCount++;
}

void DemoGame::_draw(AestheticLayer& aestheticLayer) {
    // This is the drawing logic that was previously in Engine::Run().

    // Clear the framebuffer with the background color.
    aestheticLayer.Clear(1);

    // Draw some shapes to demonstrate the API.
    aestheticLayer.RectFill(10, 10, 50, 30, 2); // Purple filled rectangle
    aestheticLayer.Rect(70, 10, 40, 40, 10);    // Yellow rectangle outline

    // Draw a diagonal line that changes color.
    uint8_t lineColor = 7 + (frameCount / 30) % 9; // Changes between white and the bright colors
    aestheticLayer.Line(10, 50, 245, 200, lineColor);

    // Draw a small moving rectangle.
    aestheticLayer.RectFill((frameCount * 2) % 246, 220, 10, 10, 8); // Red
}