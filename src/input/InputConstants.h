#ifndef INPUT_CONSTANTS_H
#define INPUT_CONSTANTS_H

#include <SDL.h>

/**
 * @file InputConstants.h
 * @brief Complete input mapping for ULICS fantasy console
 * 
 * Phase 5.19: Fantasy console 12-button layout
 * Phase 5.20: All keyboard keys and gamepad buttons (Enhanced Input)
 */

namespace ULICS {
namespace Input {

// === Fantasy Console Button IDs (Phase 5.19) ===

enum Button {
    BUTTON_LEFT  = 0,   // D-pad Left
    BUTTON_RIGHT = 1,   // D-pad Right
    BUTTON_UP    = 2,   // D-pad Up
    BUTTON_DOWN  = 3,   // D-pad Down
    BUTTON_A     = 4,   // Action button A (Z key / A button)
    BUTTON_B     = 5,   // Action button B (X key / B button)
    BUTTON_X     = 6,   // Action button X (A key / X button)
    BUTTON_Y     = 7,   // Action button Y (S key / Y button)
    BUTTON_L     = 8,   // Left shoulder (Q key / L button)
    BUTTON_R     = 9,   // Right shoulder (W key / R button)
    BUTTON_START = 10,  // Start button (Enter / Start)
    BUTTON_SELECT= 11,  // Select button (Shift / Back)
    
    BUTTON_COUNT = 12   // Total number of fantasy console buttons
};

// === Keyboard Mapping for Fantasy Console Buttons ===

const SDL_Scancode KEYBOARD_MAPPING[BUTTON_COUNT] = {
    SDL_SCANCODE_LEFT,      // BUTTON_LEFT
    SDL_SCANCODE_RIGHT,     // BUTTON_RIGHT
    SDL_SCANCODE_UP,        // BUTTON_UP
    SDL_SCANCODE_DOWN,      // BUTTON_DOWN
    SDL_SCANCODE_Z,         // BUTTON_A
    SDL_SCANCODE_X,         // BUTTON_B
    SDL_SCANCODE_A,         // BUTTON_X
    SDL_SCANCODE_S,         // BUTTON_Y
    SDL_SCANCODE_Q,         // BUTTON_L
    SDL_SCANCODE_W,         // BUTTON_R
    SDL_SCANCODE_RETURN,    // BUTTON_START
    SDL_SCANCODE_LSHIFT     // BUTTON_SELECT
};

// === Gamepad Mapping for Fantasy Console Buttons ===

const SDL_GameControllerButton GAMEPAD_MAPPING[BUTTON_COUNT] = {
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    SDL_CONTROLLER_BUTTON_DPAD_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_A,
    SDL_CONTROLLER_BUTTON_B,
    SDL_CONTROLLER_BUTTON_X,
    SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_BACK
};

// === ALL Keyboard Keys (Phase 5.20 - Enhanced Input) ===

namespace Key {
    // Letters
    const SDL_Scancode A = SDL_SCANCODE_A;
    const SDL_Scancode B = SDL_SCANCODE_B;
    const SDL_Scancode C = SDL_SCANCODE_C;
    const SDL_Scancode D = SDL_SCANCODE_D;
    const SDL_Scancode E = SDL_SCANCODE_E;
    const SDL_Scancode F = SDL_SCANCODE_F;
    const SDL_Scancode G = SDL_SCANCODE_G;
    const SDL_Scancode H = SDL_SCANCODE_H;
    const SDL_Scancode I = SDL_SCANCODE_I;
    const SDL_Scancode J = SDL_SCANCODE_J;
    const SDL_Scancode K = SDL_SCANCODE_K;
    const SDL_Scancode L = SDL_SCANCODE_L;
    const SDL_Scancode M = SDL_SCANCODE_M;
    const SDL_Scancode N = SDL_SCANCODE_N;
    const SDL_Scancode O = SDL_SCANCODE_O;
    const SDL_Scancode P = SDL_SCANCODE_P;
    const SDL_Scancode Q = SDL_SCANCODE_Q;
    const SDL_Scancode R = SDL_SCANCODE_R;
    const SDL_Scancode S = SDL_SCANCODE_S;
    const SDL_Scancode T = SDL_SCANCODE_T;
    const SDL_Scancode U = SDL_SCANCODE_U;
    const SDL_Scancode V = SDL_SCANCODE_V;
    const SDL_Scancode W = SDL_SCANCODE_W;
    const SDL_Scancode X = SDL_SCANCODE_X;
    const SDL_Scancode Y = SDL_SCANCODE_Y;
    const SDL_Scancode Z = SDL_SCANCODE_Z;
    
    // Numbers
    const SDL_Scancode NUM_1 = SDL_SCANCODE_1;
    const SDL_Scancode NUM_2 = SDL_SCANCODE_2;
    const SDL_Scancode NUM_3 = SDL_SCANCODE_3;
    const SDL_Scancode NUM_4 = SDL_SCANCODE_4;
    const SDL_Scancode NUM_5 = SDL_SCANCODE_5;
    const SDL_Scancode NUM_6 = SDL_SCANCODE_6;
    const SDL_Scancode NUM_7 = SDL_SCANCODE_7;
    const SDL_Scancode NUM_8 = SDL_SCANCODE_8;
    const SDL_Scancode NUM_9 = SDL_SCANCODE_9;
    const SDL_Scancode NUM_0 = SDL_SCANCODE_0;
    
    // Arrow Keys
    const SDL_Scancode LEFT  = SDL_SCANCODE_LEFT;
    const SDL_Scancode RIGHT = SDL_SCANCODE_RIGHT;
    const SDL_Scancode UP    = SDL_SCANCODE_UP;
    const SDL_Scancode DOWN  = SDL_SCANCODE_DOWN;
    
    // Function Keys
    const SDL_Scancode F1  = SDL_SCANCODE_F1;
    const SDL_Scancode F2  = SDL_SCANCODE_F2;
    const SDL_Scancode F3  = SDL_SCANCODE_F3;
    const SDL_Scancode F4  = SDL_SCANCODE_F4;
    const SDL_Scancode F5  = SDL_SCANCODE_F5;
    const SDL_Scancode F6  = SDL_SCANCODE_F6;
    const SDL_Scancode F7  = SDL_SCANCODE_F7;
    const SDL_Scancode F8  = SDL_SCANCODE_F8;
    const SDL_Scancode F9  = SDL_SCANCODE_F9;
    const SDL_Scancode F10 = SDL_SCANCODE_F10;
    const SDL_Scancode F11 = SDL_SCANCODE_F11;
    const SDL_Scancode F12 = SDL_SCANCODE_F12;
    
    // Modifiers
    const SDL_Scancode LSHIFT = SDL_SCANCODE_LSHIFT;
    const SDL_Scancode RSHIFT = SDL_SCANCODE_RSHIFT;
    const SDL_Scancode LCTRL  = SDL_SCANCODE_LCTRL;
    const SDL_Scancode RCTRL  = SDL_SCANCODE_RCTRL;
    const SDL_Scancode LALT   = SDL_SCANCODE_LALT;
    const SDL_Scancode RALT   = SDL_SCANCODE_RALT;
    
    // Special Keys
    const SDL_Scancode SPACE     = SDL_SCANCODE_SPACE;
    const SDL_Scancode RETURN    = SDL_SCANCODE_RETURN;
    const SDL_Scancode ESCAPE    = SDL_SCANCODE_ESCAPE;
    const SDL_Scancode BACKSPACE = SDL_SCANCODE_BACKSPACE;
    const SDL_Scancode TAB       = SDL_SCANCODE_TAB;
    const SDL_Scancode CAPSLOCK  = SDL_SCANCODE_CAPSLOCK;
    
    // Symbols
    const SDL_Scancode MINUS      = SDL_SCANCODE_MINUS;
    const SDL_Scancode EQUALS     = SDL_SCANCODE_EQUALS;
    const SDL_Scancode LEFTBRACKET  = SDL_SCANCODE_LEFTBRACKET;
    const SDL_Scancode RIGHTBRACKET = SDL_SCANCODE_RIGHTBRACKET;
    const SDL_Scancode BACKSLASH  = SDL_SCANCODE_BACKSLASH;
    const SDL_Scancode SEMICOLON  = SDL_SCANCODE_SEMICOLON;
    const SDL_Scancode APOSTROPHE = SDL_SCANCODE_APOSTROPHE;
    const SDL_Scancode GRAVE      = SDL_SCANCODE_GRAVE;
    const SDL_Scancode COMMA      = SDL_SCANCODE_COMMA;
    const SDL_Scancode PERIOD     = SDL_SCANCODE_PERIOD;
    const SDL_Scancode SLASH      = SDL_SCANCODE_SLASH;
    
    // Editing
    const SDL_Scancode INSERT   = SDL_SCANCODE_INSERT;
    const SDL_Scancode DELETE   = SDL_SCANCODE_DELETE;
    const SDL_Scancode HOME     = SDL_SCANCODE_HOME;
    const SDL_Scancode END      = SDL_SCANCODE_END;
    const SDL_Scancode PAGEUP   = SDL_SCANCODE_PAGEUP;
    const SDL_Scancode PAGEDOWN = SDL_SCANCODE_PAGEDOWN;
    
    // Numpad
    const SDL_Scancode KP_0 = SDL_SCANCODE_KP_0;
    const SDL_Scancode KP_1 = SDL_SCANCODE_KP_1;
    const SDL_Scancode KP_2 = SDL_SCANCODE_KP_2;
    const SDL_Scancode KP_3 = SDL_SCANCODE_KP_3;
    const SDL_Scancode KP_4 = SDL_SCANCODE_KP_4;
    const SDL_Scancode KP_5 = SDL_SCANCODE_KP_5;
    const SDL_Scancode KP_6 = SDL_SCANCODE_KP_6;
    const SDL_Scancode KP_7 = SDL_SCANCODE_KP_7;
    const SDL_Scancode KP_8 = SDL_SCANCODE_KP_8;
    const SDL_Scancode KP_9 = SDL_SCANCODE_KP_9;
    const SDL_Scancode KP_DIVIDE   = SDL_SCANCODE_KP_DIVIDE;
    const SDL_Scancode KP_MULTIPLY = SDL_SCANCODE_KP_MULTIPLY;
    const SDL_Scancode KP_MINUS    = SDL_SCANCODE_KP_MINUS;
    const SDL_Scancode KP_PLUS     = SDL_SCANCODE_KP_PLUS;
    const SDL_Scancode KP_ENTER    = SDL_SCANCODE_KP_ENTER;
    const SDL_Scancode KP_PERIOD   = SDL_SCANCODE_KP_PERIOD;
}

// === ALL Gamepad Buttons (Phase 5.20 - Enhanced Input) ===

namespace GamepadButton {
    // Face Buttons (Xbox layout)
    const SDL_GameControllerButton A = SDL_CONTROLLER_BUTTON_A;
    const SDL_GameControllerButton B = SDL_CONTROLLER_BUTTON_B;
    const SDL_GameControllerButton X = SDL_CONTROLLER_BUTTON_X;
    const SDL_GameControllerButton Y = SDL_CONTROLLER_BUTTON_Y;
    
    // D-pad
    const SDL_GameControllerButton DPAD_UP    = SDL_CONTROLLER_BUTTON_DPAD_UP;
    const SDL_GameControllerButton DPAD_DOWN  = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    const SDL_GameControllerButton DPAD_LEFT  = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    const SDL_GameControllerButton DPAD_RIGHT = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    
    // Shoulders
    const SDL_GameControllerButton LEFT_SHOULDER  = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    const SDL_GameControllerButton RIGHT_SHOULDER = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    
    // Stick Buttons
    const SDL_GameControllerButton LEFT_STICK  = SDL_CONTROLLER_BUTTON_LEFTSTICK;
    const SDL_GameControllerButton RIGHT_STICK = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    
    // System Buttons
    const SDL_GameControllerButton START = SDL_CONTROLLER_BUTTON_START;
    const SDL_GameControllerButton BACK  = SDL_CONTROLLER_BUTTON_BACK;
    const SDL_GameControllerButton GUIDE = SDL_CONTROLLER_BUTTON_GUIDE;
    
    // Special Buttons (Xbox Elite, PS5, etc.)
    const SDL_GameControllerButton PADDLE1 = SDL_CONTROLLER_BUTTON_PADDLE1;
    const SDL_GameControllerButton PADDLE2 = SDL_CONTROLLER_BUTTON_PADDLE2;
    const SDL_GameControllerButton PADDLE3 = SDL_CONTROLLER_BUTTON_PADDLE3;
    const SDL_GameControllerButton PADDLE4 = SDL_CONTROLLER_BUTTON_PADDLE4;
    const SDL_GameControllerButton TOUCHPAD = SDL_CONTROLLER_BUTTON_TOUCHPAD;
    
    const int COUNT = SDL_CONTROLLER_BUTTON_MAX;  // Total: 21 buttons
}

// === Analog Axes (Phase 5.20) ===

namespace GamepadAxis {
    const SDL_GameControllerAxis LEFT_X  = SDL_CONTROLLER_AXIS_LEFTX;
    const SDL_GameControllerAxis LEFT_Y  = SDL_CONTROLLER_AXIS_LEFTY;
    const SDL_GameControllerAxis RIGHT_X = SDL_CONTROLLER_AXIS_RIGHTX;
    const SDL_GameControllerAxis RIGHT_Y = SDL_CONTROLLER_AXIS_RIGHTY;
    const SDL_GameControllerAxis TRIGGER_LEFT  = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
    const SDL_GameControllerAxis TRIGGER_RIGHT = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
    
    const int COUNT = SDL_CONTROLLER_AXIS_MAX;  // Total: 6 axes
}

// === Deadzone Constants ===

const int ANALOG_DEADZONE = 8000;   // ~24% of max range (32767)
const int TRIGGER_DEADZONE = 3000;  // ~9% of max range

// === Button Names for Debug ===

inline const char* BUTTON_NAMES[BUTTON_COUNT] = {
    "Left", "Right", "Up", "Down",
    "A", "B", "X", "Y",
    "L", "R", "Start", "Select"
};

} // namespace Input
} // namespace ULICS

#endif // INPUT_CONSTANTS_H
