#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL.h>
#include <vector>

/// @class InputManager
/// @brief Manages keyboard input state for the engine.
///
/// This class tracks the current and previous state of the keyboard to allow
/// for querying both "key down" (held) and "key pressed" (just pressed this frame) states.
class InputManager {
public:
    InputManager();

    /// @brief Prepares the manager for a new frame.
    /// This should be called once at the beginning of the main loop, before event polling.
    void beginNewFrame();

    /// @brief Processes an SDL keyboard event.
    /// This should be called for each SDL_KEYDOWN and SDL_KEYUP event.
    void handleKeyEvent(const SDL_Event& event);

    /// @brief Checks if a key is currently held down.
    /// @param scancode The SDL_Scancode of the key to check.
    /// @return True if the key is held down, false otherwise.
    bool isKeyDown(SDL_Scancode scancode) const;

    /// @brief Checks if a key was just pressed in the current frame.
    /// @param scancode The SDL_Scancode of the key to check.
    /// @return True if the key was pressed this frame, false otherwise.
    bool isKeyPressed(SDL_Scancode scancode) const;

private:
    std::vector<Uint8> previousKeyStates;
    const Uint8* currentKeyStates;
    int numKeys;
};

#endif // INPUT_MANAGER_H
