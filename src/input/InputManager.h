#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL.h>
#include <vector>
#include <array>
#include <memory>

/// @class InputManager
/// @brief Manages keyboard input state for the engine.
///
/// This class tracks the current and previous state of the keyboard to allow
/// for querying both "key down" (held) and "key pressed" (just pressed this frame) states.
class InputManager {
public:
    InputManager();

    /// @brief Updates the internal state of the input manager.
    /// This should be called once per frame, before the game logic.
    void update();
    
    // Controller management
    void addController(int deviceIndex);
    void removeController(int instanceId);

    /// @brief Processes an SDL keyboard event (kept for future use).
    void handleKeyEvent(const SDL_Event& event);

    /// @brief Checks if a key is currently held down.    
    bool isKeyDown(SDL_Scancode scancode) const;

    /// @brief Checks if a key was just pressed in the current frame.    
    bool isKeyPressed(SDL_Scancode scancode) const;

private:
    // Player 0 (Keyboard) state
    std::vector<Uint8> previousKeyStates; // State from the previous frame.
    std::vector<Uint8> currentKeyStates;  // State for the current frame.

    // Gamepad state
    static constexpr int MAX_CONTROLLERS = 4;
    struct ControllerState {
        SDL_GameController* handle = nullptr;
        std::array<Uint8, SDL_CONTROLLER_BUTTON_MAX> previousButtons{};
        std::array<Uint8, SDL_CONTROLLER_BUTTON_MAX> currentButtons{};
    };
    std::array<ControllerState, MAX_CONTROLLERS> controllers;

public: // Public for Lua bindings, but should be treated as internal
    bool isGamepadButtonDown(int playerIndex, int button) const;
    bool isGamepadButtonPressed(int playerIndex, int button) const;
};

#endif // INPUT_MANAGER_H
