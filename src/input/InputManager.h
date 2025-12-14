#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL.h>
#include <vector>
#include <string>  // For text input (Phase 2.0.2)

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

    // === Mouse Input (Phase 5.16) ===

    /// @brief Processes an SDL mouse event.
    void handleMouseEvent(const SDL_Event& event);

    /// @brief Get mouse X position (screen coordinates)
    int getMouseX() const { return mouseX; }

    /// @brief Get mouse Y position (screen coordinates)
    int getMouseY() const { return mouseY; }

    /// @brief Check if left mouse button is held down
    bool isMouseButtonDown(int button) const;

    /// @brief Check if left mouse button was just pressed this frame
    bool isMouseButtonPressed(int button) const;

    // === Gamepad Input (Phase 5.18) ===

    /// @brief Processes SDL gamepad events
    void handleGamepadEvent(const SDL_Event& event);

    /// @brief Check if a gamepad button is held down (for first connected controller)
    /// @param button SDL_GameControllerButton enum value
    bool isGamepadButtonDown(int button) const;

    /// @brief Check if a gamepad button was just pressed this frame
    bool isGamepadButtonPressed(int button) const;

    /// @brief Get number of connected gamepads
    int getNumGamepads() const { return numGamepads; }

    // === Analog Stick/Trigger Reading (Phase 5.20) ===

    /// @brief Get analog axis value (-32768 to 32767, or 0 to 32767 for triggers)
    /// @param axis SDL_GameControllerAxis enum value
    /// @return Raw axis value, or 0 if no gamepad connected
    Sint16 getGamepadAxis(int axis) const;

    /// @brief Get analog axis value normalized to -1.0 to 1.0 (or 0.0 to 1.0 for triggers)
    /// @param axis SDL_GameControllerAxis enum value
    /// @param applyDeadzone If true, applies deadzone (values near 0 become 0.0)
    /// @return Normalized axis value
    float getGamepadAxisNormalized(int axis, bool applyDeadzone = true) const;

    // === Text Input (Phase 2.0.2 - Code Editor) ===

    /// @brief Handle SDL_TEXTINPUT event
    void handleTextInput(const SDL_Event& event);

    /// @brief Get text input from this frame (for text editors)
    /// @return String containing all text typed this frame
    const std::string& getTextInput() const { return textInput; }

    /// @brief Check if text input has content this frame
    bool hasTextInput() const { return !textInput.empty(); }

    // === Modifier Keys (Phase 2.0.4) ===

    /// @brief Check if Ctrl key is currently held down
    bool isCtrlDown() const {
        return isKeyDown(SDL_SCANCODE_LCTRL) || isKeyDown(SDL_SCANCODE_RCTRL);
    }

    /// @brief Check if Shift key is currently held down
    bool isShiftDown() const {
        return isKeyDown(SDL_SCANCODE_LSHIFT) || isKeyDown(SDL_SCANCODE_RSHIFT);
    }

private:
    // Keyboard state
    std::vector<Uint8> previousKeyStates;
    const Uint8* currentKeyStates;
    int numKeys;

    // Mouse state (Phase 5.16)
    int mouseX;
    int mouseY;
    Uint32 currentMouseButtons;
    Uint32 previousMouseButtons;

    // Gamepad state (Phase 5.18)
    SDL_GameController* gameController;
    int numGamepads;
    std::vector<Uint8> previousGamepadButtons;
    std::vector<Uint8> currentGamepadButtons;

    // Text input (Phase 2.0.2 - Code Editor)
    std::string textInput;  // Text typed this frame
};

#endif // INPUT_MANAGER_H
