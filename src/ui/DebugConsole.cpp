#include "ui/DebugConsole.h"
#include "rendering/AestheticLayer.h"
#include <sstream>
#include <iomanip>

DebugConsole::DebugConsole() 
    : visible(false), currentFPS(60.0), fpsAccumulator(0.0), frameCount(0) {
    lastFPSUpdate = std::chrono::high_resolution_clock::now();
}

DebugConsole::~DebugConsole() {
    // Cleanup
}

void DebugConsole::AddMessage(const std::string& message) {
    messages.push_back(message);
    
    // Keep only last MAX_MESSAGES
    if (messages.size() > MAX_MESSAGES) {
        messages.erase(messages.begin());
    }
}

void DebugConsole::UpdateFPS(double deltaTime) {
    frameCount++;
    fpsAccumulator += deltaTime;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFPSUpdate).count();
    
    // Update FPS every 500ms
    if (elapsed >= 500) {
        if (fpsAccumulator > 0) {
            currentFPS = frameCount / (fpsAccumulator / 1000.0);
        }
        frameCount = 0;
        fpsAccumulator = 0.0;
        lastFPSUpdate = now;
    }
}

void DebugConsole::Draw(AestheticLayer& layer) {
    if (!visible) {
        return;
    }
    
    // Draw semi-transparent background using rectfill
    int consoleHeight = 10 + (messages.size() + 2) * 7;
    layer.RectFill(0, 0, 255, consoleHeight, 1);  // Dark blue bg
    
    // Draw header
    layer.Print("DEBUG (F1)", 2, 2, 7);  // White text
    
    // Draw FPS
    std::ostringstream fpsText;
    fpsText << "FPS:" << std::fixed << std::setprecision(0) << currentFPS;
    layer.Print(fpsText.str(), 2, 9, 11);  // Green
    
    // Draw messages
    int y = 16;
    for (const auto& msg : messages) {
        // Truncate if too long (50 chars max at 4px font = ~200px)
        std::string displayMsg = msg.length() > 40 ? msg.substr(0, 37) + "..." : msg;
        layer.Print(displayMsg, 2, y, 6);  // Light gray
        y += 7;
    }
}

void DebugConsole::Clear() {
    messages.clear();
}
