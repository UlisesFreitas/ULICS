#ifndef DEBUGCONSOLE_H
#define DEBUGCONSOLE_H

#include <string>
#include <vector>
#include <chrono>

class AestheticLayer;

/**
 * @brief DebugConsole - On-screen debug overlay
 * 
 * Shows:
 * - Last 10 print() messages from Lua
 * - FPS counter
 * - Memory/resource usage
 * 
 * Toggle with F1 key
 */
class DebugConsole {
public:
    DebugConsole();
    ~DebugConsole();

    /**
     * @brief Add a message to the console
     */
    void AddMessage(const std::string& message);

    /**
     * @brief Update FPS calculation
     */
    void UpdateFPS(double deltaTime);

    /**
     * @brief Draw the console overlay
     */
    void Draw(AestheticLayer& layer);

    /**
     * @brief Toggle console visibility
     */
    void Toggle() { visible = !visible; }
    void SetVisible(bool vis) { visible = vis; }
    bool IsVisible() const { return visible; }

    /**
     * @brief Clear all messages
     */
    void Clear();

private:
    bool visible;
    std::vector<std::string> messages;
    static constexpr int MAX_MESSAGES = 10;
    
    // FPS tracking
    double currentFPS;
    double fpsAccumulator;
    int frameCount;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFPSUpdate;
};

#endif // DEBUGCONSOLE_H
