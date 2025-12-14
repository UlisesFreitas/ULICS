#include "input/InputManager.h"
#include "input/InputConstants.h"
#include <cstring> // For memcpy
#include <iostream>

InputManager::InputManager() 
    : mouseX(0)
    , mouseY(0)
    , mouseWheelY(0)
    , currentMouseButtons(0)
    , previousMouseButtons(0)
    , mouseScaleX(1.0f)
    , mouseScaleY(1.0f)
    , gameController(nullptr)
    , numGamepads(0) {
    // Get a pointer to SDL's internal keyboard state array.
    // SDL manages the memory for this array.
    currentKeyStates = SDL_GetKeyboardState(&numKeys);

    // Allocate our own array to store the previous frame's state.
    previousKeyStates.resize(numKeys);

    // Initialize the previous state to the current state.
    memcpy(previousKeyStates.data(), currentKeyStates, numKeys);

    // Initialize gamepad button state arrays
    // SDL_GameController has SDL_CONTROLLER_BUTTON_MAX buttons
    previousGamepadButtons.resize(SDL_CONTROLLER_BUTTON_MAX, 0);
    currentGamepadButtons.resize(SDL_CONTROLLER_BUTTON_MAX, 0);

    // Try to open the first available gamepad (Phase 5.18)
    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; i++) {
        if (SDL_IsGameController(i)) {
            gameController = SDL_GameControllerOpen(i);
            if (gameController) {
                numGamepads = 1;
                std::cout << "InputManager: Gamepad connected: " 
                          << SDL_GameControllerName(gameController) << std::endl;
                break;
            }
        }
    }

    if (!gameController) {
        std::cout << "InputManager: No gamepad detected" << std::endl;
    }
}

void InputManager::beginNewFrame() {
    // Copy the current state to the previous state array before polling for new events.
    memcpy(previousKeyStates.data(), currentKeyStates, numKeys);
    
    // Copy current mouse button state to previous
    previousMouseButtons = currentMouseButtons;
    
    // Reset mouse wheel delta each frame
    mouseWheelY = 0;

    // Copy current gamepad button state to previous
    previousGamepadButtons = currentGamepadButtons;
    
    // Update gamepad button state (poll every frame, not just on events)
    if (gameController) {
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            currentGamepadButtons[i] = SDL_GameControllerGetButton(
                gameController, static_cast<SDL_GameControllerButton>(i));
        }
    }

    // Clear text input from previous frame (Phase 2.0.2)
    textInput.clear();
}

void InputManager::handleKeyEvent(const SDL_Event& event) {
    // SDL_GetKeyboardState is updated automatically after SDL_PollEvent,
    // so we don't need to manually handle key up/down events here.
    // This function is kept for potential future use (e.g., text input).
    (void)event;
}

void InputManager::handleMouseEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_MOUSEMOTION:
            // SDL_RenderSetLogicalSize handles coordinate conversion automatically
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            break;
            
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            // SDL_RenderSetLogicalSize handles coordinate conversion automatically
            mouseX = event.button.x;
            mouseY = event.button.y;
            break;
            
        case SDL_MOUSEWHEEL:
            // Positive = scroll up, negative = scroll down
            mouseWheelY = event.wheel.y;
            break;
            
        default:
            break;
    }
    
    // Update current mouse button state
    currentMouseButtons = SDL_GetMouseState(nullptr, nullptr);
}

void InputManager::handleGamepadEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_CONTROLLERDEVICEADDED:
            // A new controller was connected
            if (!gameController && SDL_IsGameController(event.cdevice.which)) {
                gameController = SDL_GameControllerOpen(event.cdevice.which);
                if (gameController) {
                    numGamepads = 1;
                    std::cout << "InputManager: Gamepad connected: " 
                              << SDL_GameControllerName(gameController) << std::endl;
                }
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            // A controller was disconnected
            if (gameController && event.cdevice.which == SDL_JoystickInstanceID(
                SDL_GameControllerGetJoystick(gameController))) {
                SDL_GameControllerClose(gameController);
                gameController = nullptr;
                numGamepads = 0;
                std::cout << "InputManager: Gamepad disconnected" << std::endl;
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            // Update gamepad button state
            if (gameController) {
                for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
                    currentGamepadButtons[i] = SDL_GameControllerGetButton(
                        gameController, static_cast<SDL_GameControllerButton>(i));
                }
            }
            break;

        default:
            break;
    }
}

bool InputManager::isKeyDown(SDL_Scancode scancode) const {
    return currentKeyStates[scancode] == 1;
}

bool InputManager::isKeyPressed(SDL_Scancode scancode) const {
    // A key is "pressed" if it's down now but was not down in the previous frame.
    return currentKeyStates[scancode] == 1 && previousKeyStates[scancode] == 0;
}

bool InputManager::isMouseButtonDown(int button) const {
    // button: 1=left, 2=middle, 3=right (SDL_BUTTON_*)
    Uint32 mask = SDL_BUTTON(button);
    return (currentMouseButtons & mask) != 0;
}

bool InputManager::isMouseButtonPressed(int button) const {
    // Button was just pressed this frame
    Uint32 mask = SDL_BUTTON(button);
    bool currentlyDown = (currentMouseButtons & mask) != 0;
    bool previouslyDown = (previousMouseButtons & mask) != 0;
    return currentlyDown && !previouslyDown;
}

bool InputManager::isGamepadButtonDown(int button) const {
    if (!gameController || button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX) {
        return false;
    }
    return currentGamepadButtons[button] != 0;
}

bool InputManager::isGamepadButtonPressed(int button) const {
    if (!gameController || button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX) {
        return false;
    }
    bool currentlyDown = currentGamepadButtons[button] != 0;
    bool previouslyDown = previousGamepadButtons[button] != 0;
    return currentlyDown && !previouslyDown;
}

// === Analog Stick/Trigger Reading (Phase 5.20) ===

Sint16 InputManager::getGamepadAxis(int axis) const {
    if (!gameController || axis < 0 || axis >= SDL_CONTROLLER_AXIS_MAX) {
        return 0;
    }
    return SDL_GameControllerGetAxis(gameController, static_cast<SDL_GameControllerAxis>(axis));
}

float InputManager::getGamepadAxisNormalized(int axis, bool applyDeadzone) const {
    using namespace ULICS::Input;
    
    if (!gameController) {
        return 0.0f;
    }
    
    Sint16 rawValue = getGamepadAxis(axis);
    
    // Triggers are 0 to 32767, sticks are -32768 to 32767
    bool isTrigger = (axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT || 
                      axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
    
    if (applyDeadzone) {
        int deadzone = isTrigger ? TRIGGER_DEADZONE : ANALOG_DEADZONE;
        
        if (isTrigger) {
            // Trigger: 0 to 32767
            if (rawValue < deadzone) {
                return 0.0f;
            }
            // Normalize to 0.0 - 1.0
            return static_cast<float>(rawValue - deadzone) / (32767.0f - deadzone);
        } else {
            // Stick: -32768 to 32767
            if (rawValue > -deadzone && rawValue < deadzone) {
                return 0.0f;
            }
            // Normalize to -1.0 to 1.0
            if (rawValue > 0) {
                return static_cast<float>(rawValue - deadzone) / (32767.0f - deadzone);
            } else {
                return static_cast<float>(rawValue + deadzone) / (32768.0f - deadzone);
            }
        }
    } else {
        // No deadzone, just normalize
        if (isTrigger) {
            return rawValue / 32767.0f;
        } else {
            return rawValue / (rawValue > 0 ? 32767.0f : 32768.0f);
        }
    }
}

// === Text Input (Phase 2.0.2 - Code Editor) ===

void InputManager::handleTextInput(const SDL_Event& event) {
    if (event.type == SDL_TEXTINPUT) {
        // Append text (SDL_TEXTINPUT can have multiple UTF-8 chars)
        textInput += event.text.text;
    }
}