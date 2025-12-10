#include "input/InputManager.h"
#include <cstring> // For memcpy
#include <iostream>

InputManager::InputManager() {
    // Initialize the keyboard state arrays.
    // We perform an initial poll to populate the current state.
    SDL_PumpEvents();
    const Uint8* sdlKeyStates = SDL_GetKeyboardState(nullptr);
    
    // Allocate our own buffers and copy the initial state into both.
    // We use SDL_NUM_SCANCODES as it's the definitive size of the array.
    currentKeyStates.assign(sdlKeyStates, sdlKeyStates + SDL_NUM_SCANCODES);
    previousKeyStates = currentKeyStates;

    for (auto& controller : controllers) {
        controller.handle = nullptr;
    }
}

void InputManager::update() {
    // 1. Copy the now-old current state to the previous state buffer.
    previousKeyStates = currentKeyStates;
    
    // 2. Poll for new events to update SDL's internal state.
    SDL_PumpEvents();
    
    // 3. Copy the new state into our current state buffer.
    const Uint8* sdlKeyStates = SDL_GetKeyboardState(nullptr);
    currentKeyStates.assign(sdlKeyStates, sdlKeyStates + SDL_NUM_SCANCODES);

    // 4. Update all active controllers
    for (auto& controller : controllers) {
        if (controller.handle) {
            controller.previousButtons = controller.currentButtons;
            for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
                controller.currentButtons[i] = SDL_GameControllerGetButton(controller.handle, (SDL_GameControllerButton)i);
            }
        }
    }
}

void InputManager::addController(int deviceIndex) {
    if (SDL_IsGameController(deviceIndex)) {
        for (int i = 0; i < MAX_CONTROLLERS; ++i) {
            if (!controllers[i].handle) {
                controllers[i].handle = SDL_GameControllerOpen(deviceIndex);
                if (controllers[i].handle) {
                    std::cout << "InputManager: Controller '" << SDL_GameControllerName(controllers[i].handle) << "' connected to player " << i + 1 << std::endl;
                }
                return;
            }
        }
        std::cout << "InputManager: Could not add controller, max players reached." << std::endl;
    }
}

void InputManager::removeController(int instanceId) {
    for (int i = 0; i < MAX_CONTROLLERS; ++i) {
        if (controllers[i].handle && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[i].handle)) == instanceId) {
            SDL_GameControllerClose(controllers[i].handle);
            controllers[i].handle = nullptr;
            std::cout << "InputManager: Controller for player " << i + 1 << " disconnected." << std::endl;
            return;
        }
    }
}

void InputManager::handleKeyEvent(const SDL_Event& event) {
    // SDL_GetKeyboardState is updated automatically after SDL_PollEvent,
    // so we don't need to manually handle key up/down events here.
    // This function is kept for potential future use (e.g., text input).
    (void)event;
}

bool InputManager::isKeyDown(SDL_Scancode scancode) const {
    return currentKeyStates.at(scancode) == 1;
}

bool InputManager::isKeyPressed(SDL_Scancode scancode) const {
    // A key is "pressed" if it's down now but was not down in the previous frame.
    return currentKeyStates.at(scancode) == 1 && previousKeyStates.at(scancode) == 0;
}

bool InputManager::isGamepadButtonDown(int playerIndex, int button) const {
    if (playerIndex < 1 || playerIndex > MAX_CONTROLLERS) return false;
    const auto& controller = controllers[playerIndex - 1];

    if (controller.handle && button >= 0 && button < SDL_CONTROLLER_BUTTON_MAX) {
        return controller.currentButtons[button] == 1;
    }
    return false;
}

bool InputManager::isGamepadButtonPressed(int playerIndex, int button) const {
    if (playerIndex < 1 || playerIndex > MAX_CONTROLLERS) return false;
    const auto& controller = controllers[playerIndex - 1];

    if (controller.handle && button >= 0 && button < SDL_CONTROLLER_BUTTON_MAX) {
        return controller.currentButtons[button] == 1 && controller.previousButtons[button] == 0;
    }
    return false;
}