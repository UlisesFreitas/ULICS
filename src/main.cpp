// Main entry point for the UliCS Fantasy Console.
// This file initializes the Engine and runs the main application loop.

#include "Engine.h"

// The cross-platform entry point for an SDL application.
int main(int argc, char* argv[]) {
    // Unused parameters.
    (void)argc;
    (void)argv;

    Engine engine;
    if (engine.Initialize()) {
        engine.Run();
    }

    return 0;
}
