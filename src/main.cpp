// Main entry point for the UliCS Fantasy Console.
// This file initializes the Engine and runs the main application loop.

#include "core/Engine.h"
#include <iostream>
#include <string>

// The cross-platform entry point for an SDL application.
int main(int argc, char* argv[]) {
    Engine engine;
    
    // Parse command-line arguments
    std::string cartridgePath = "";
    
    if (argc > 1) {
        cartridgePath = argv[1];
        std::cout << "Cartridge path provided: " << cartridgePath << std::endl;
    } else {
        std::cout << "No cartridge specified. Will load embedded demo." << std::endl;
    }
    
    // Initialize the engine with 800x800 window (better screen fit)
    // Internal framebuffer is still 256x256, this is just the window size
    if (engine.Initialize("UliCS Fantasy Console", 800, 800, cartridgePath)) {
        engine.Run();
    }

    return 0;
}
