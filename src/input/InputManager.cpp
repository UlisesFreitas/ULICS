#include "input/InputManager.h"
#include <cstring> // For memcpy

InputManager::InputManager() {
    // Get a pointer to SDL's internal keyboard state array.
    // SDL manages the memory for this array.
    currentKeyStates = SDL_GetKeyboardState(&numKeys);

    // Allocate our own array to store the previous frame's state.
    previousKeyStates.resize(numKeys);

    // Initialize the previous state to the current state.
    memcpy(previousKeyStates.data(), currentKeyStates, numKeys);
}

void InputManager::beginNewFrame() {
    // Copy the current state to the previous state array before polling for new events.
    memcpy(previousKeyStates.data(), currentKeyStates, numKeys);
}

void InputManager::handleKeyEvent(const SDL_Event& event) {
    // SDL_GetKeyboardState is updated automatically after SDL_PollEvent,
    // so we don't need to manually handle key up/down events here.
    // This function is kept for potential future use (e.g., text input).
    (void)event;
}

bool InputManager::isKeyDown(SDL_Scancode scancode) const {
    return currentKeyStates[scancode] == 1;
}

bool InputManager::isKeyPressed(SDL_Scancode scancode) const {
    // A key is "pressed" if it's down now but was not down in the previous frame.
    return currentKeyStates[scancode] == 1 && previousKeyStates[scancode] == 0;
}