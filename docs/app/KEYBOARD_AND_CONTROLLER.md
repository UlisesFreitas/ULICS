# ULICS Keyboard and Controller Input Reference

Complete reference for all supported input methods in ULICS fantasy console.

## Overview

ULICS supports three input methods:
1. **Keyboard** - Standard QWERTY keyboard
2. **Mouse** - Position and 3 buttons
3. **Gamepad** - Xbox/PlayStation/Nintendo compatible controllers

All input is automatically mapped to a unified **12-button fantasy console layout**.

---

## Fantasy Console Button Layout

ULICS uses a standardized 12-button layout:

```
D-Pad:          Action Buttons:     Shoulders:      System:
┌───┬───┐       ┌───┬───┐          L       R       Start
│ ↑ │   │       │ X │ Y │          (8)    (9)      (10)
├───┼───┤       ├───┼───┤                           Select
│← ●→│   │       │ A │ B │                           (11)
├───┼───┤       └───┴───┘
│ ↓ │   │
└───┴───┘
(0-3)           (4-7)
```

### Button IDs

| ID | Name   | Purpose              | Keyboard | Gamepad       |
|----|--------|----------------------|----------|---------------|
| 0  | LEFT   | D-pad Left           | ←        | D-pad Left    |
| 1  | RIGHT  | D-pad Right          | →        | D-pad Right   |
| 2  | UP     | D-pad Up             | ↑        | D-pad Up      |
| 3  | DOWN   | D-pad Down           | ↓        | D-pad Down    |
| 4  | A      | Primary Action       | Z        | A button      |
| 5  | B      | Secondary Action     | X        | B button      |
| 6  | X      | Tertiary Action      | A        | X button      |
| 7  | Y      | Quaternary Action    | S        | Y button      |
| 8  | L      | Left Shoulder        | Q        | L/LB button   |
| 9  | R      | Right Shoulder       | W        | R/RB button   |
| 10 | START  | Pause/Start          | Enter    | Start         |
| 11 | SELECT | Select/Back          | LShift   | Back/Select   |

---

## Keyboard Controls

### Movement (D-pad)
```
Arrow Keys:
  ↑  = UP    (Button 2)
← ● → = LEFT/RIGHT (Buttons 0, 1)
  ↓  = DOWN  (Button 3)
```

### Action Buttons
```
Z = Button A (4) - Primary action (confirm, jump)
X = Button B (5) - Secondary action (cancel, attack)
A = Button X (6) - Third action
S = Button Y (7) - Fourth action
```

### Shoulder Buttons
```
Q = Button L (8) - Left shoulder
W = Button R (9) - Right shoulder
```

### System Buttons
```
Enter  = START (10)  - Pause menu
LShift = SELECT (11) - Inventory/Map
```

### Full Keyboard Map
```
  Q   W        (Shoulders)
┌───┐
│ ↑ │         (D-pad)
├─┼─┤
│← →│
└─┼─┘
  ↓

Z X A S       (Action buttons)
Enter  Shift  (System)
```

---

## Gamepad Controls

### Supported Controllers
- ✅ Xbox (Xbox One, Series X/S, 360)
- ✅ PlayStation (DualShock 4, DualSense)
- ✅ Nintendo (Switch Pro Controller, Joy-Cons)
- ✅ Generic USB/Bluetooth controllers

### Button Mapping

**Xbox Layout:**
```
      LB(8)              RB(9)
      
D-pad     Y(7)
(0-3)   X(6) B(5)
          A(4)
          
    Back(11)  Start(10)
```

**PlayStation Layout:**
```
      L1(8)              R1(9)
      
D-pad     △(7)
(0-3)   □(6) ○(5)
          ✕(4)
          
    Select(11)  Start(10)
```

**Nintendo Layout:**
```
      L(8)               R(9)
      
D-pad     X(7)
(0-3)   Y(6) A(5)
          B(4)
          
    -(11)     +(10)
```

### Analog Sticks (Advanced)

ULICS supports analog stick input for advanced games:

```cpp
// Left Stick
AXIS_LEFT_X  = Horizontal (-32768 to 32767)
AXIS_LEFT_Y  = Vertical   (-32768 to 32767)

// Right Stick  
AXIS_RIGHT_X = Horizontal
AXIS_RIGHT_Y = Vertical

// Triggers
AXIS_TRIGGER_LEFT  = L2/LT (0 to 32767)
AXIS_TRIGGER_RIGHT = R2/RT (0 to 32767)
```

**Deadzone:** 8000 (~24% of max range)

---

## Mouse Controls

### Mouse Functions

```lua
-- Get complete mouse state
m = mouse()
print(m.x, m.y)           -- Position
if m.left then ... end    -- Left button
if m.right then ... end   -- Right button
if m.middle then ... end  -- Middle button

-- Get individual coordinates
x = mousex()
y = mousey()
```

### Mouse Buttons
- **Left** (Button 1) - Primary click
- **Right** (Button 3) - Secondary click/context menu
- **Middle** (Button 2) - Middle click/scroll wheel

---

## Lua API

### Button Input

```lua
-- Check if button is held down
if btn(0) then
    -- Left button is held
end

-- Check if button was just pressed (single frame)
if btnp(4) then
    -- A button was just pressed
end
```

### Button Constants

Since Lua doesn't have constants, use these values:

```lua
local BTN_LEFT   = 0
local BTN_RIGHT  = 1
local BTN_UP     = 2
local BTN_DOWN   = 3
local BTN_A      = 4
local BTN_B      = 5
local BTN_X      = 6
local BTN_Y      = 7
local BTN_L      = 8
local BTN_R      = 9
local BTN_START  = 10
local BTN_SELECT = 11

-- Usage
if btnp(BTN_A) then
    jump()
end
```

### Common Patterns

**Movement:**
```lua
function _update()
    if btn(BTN_LEFT) then
        player.x = player.x - 2
    end
    if btn(BTN_RIGHT) then
        player.x = player.x + 2
    end
end
```

**Jump:**
```lua
function _update()
    if btnp(BTN_A) and player.on_ground then
        player.vy = -5
    end
end
```

**Pause Menu:**
```lua
function _update()
    if btnp(BTN_START) then
        paused = not paused
    end
end
```

---

## Hot-Plug Support

ULICS automatically detects controllers:

- ✅ **Connect during gameplay** - Works immediately
- ✅ **Disconnect during gameplay** - Falls back to keyboard
- ✅ **Multiple controllers** - First controller used

Console output:
```
InputManager: Gamepad connected: Xbox Controller
InputManager: Gamepad disconnected
```

---

## Technical Details

### Input Priority
When both keyboard and gamepad are active:
```
btn() returns true if EITHER keyboard OR gamepad button is pressed
```

This allows seamless switching between input methods.

### Polling Rate
- **Keyboard/Gamepad:** Polled every frame (~60 FPS)
- **Mouse:** Event-driven + polled

### Button State
- `btn()` - Returns true while button is held
- `btnp()` - Returns true only on first frame of press

---

## Example: Complete Input Test

```lua
function _draw()
    cls(0)
    
    -- D-pad
    if btn(BTN_LEFT) then print("LEFT", 10, 10, 11) end
    if btn(BTN_RIGHT) then print("RIGHT", 10, 20, 11) end
    if btn(BTN_UP) then print("UP", 10, 30, 11) end
    if btn(BTN_DOWN) then print("DOWN", 10, 40, 11) end
    
    -- Action buttons
    if btn(BTN_A) then print("A", 60, 10, 12) end
    if btn(BTN_B) then print("B", 60, 20, 12) end
    if btn(BTN_X) then print("X", 60, 30, 12) end
    if btn(BTN_Y) then print("Y", 60, 40, 12) end
    
    -- Shoulders
    if btn(BTN_L) then print("L", 110, 10, 10) end
    if btn(BTN_R) then print("R", 110, 20, 10) end
    
    -- System
    if btn(BTN_START) then print("START", 110, 30, 14) end
    if btn(BTN_SELECT) then print("SELECT", 110, 40, 14) end
end
```

---

## Best Practices

### 1. Use Button Constants
```lua
-- Good
if btnp(BTN_A) then jump() end

-- Avoid
if btnp(4) then jump() end
```

### 2. Separate Input from Logic
```lua
-- Good
function handle_input()
    local move_x = 0
    if btn(BTN_LEFT) then move_x = -1 end
    if btn(BTN_RIGHT) then move_x = 1 end
    return move_x
end

function _update()
    local dx = handle_input()
    player.x = player.x + dx * player.speed
end
```

### 3. Use btnp() for Menu Navigation
```lua
-- Menus should use btnp() to avoid rapid scrolling
if btnp(BTN_UP) then
    menu_index = menu_index - 1
end
```

### 4. Use btn() for Continuous Actions
```lua
-- Movement should use btn() for smooth control
if btn(BTN_A) then
    player.boost = true
end
```

---

## Advanced Input (Phase 5.20 - Enhanced)

### Complete Keyboard Access

ULICS provides access to **100+ keyboard keys** beyond the fantasy console buttons:

```cpp
// In C++ code using ULICS::Input::Key namespace
Key::A-Z          // All letters
Key::NUM_0-9      // Number row
Key::F1-F12       // Function keys
Key::KP_0-9       // Numpad
Key::SPACE, RETURN, ESCAPE, TAB
Key::LSHIFT, RSHIFT, LCTRL, RCTRL, LALT, RALT
// And many more...
```

### All Gamepad Buttons

Access to **21 gamepad buttons** including special buttons:

```cpp
// Face buttons
GamepadButton::A, B, X, Y

// D-pad
GamepadButton::DPAD_UP, DOWN, LEFT, RIGHT

// Shoulders
GamepadButton::LEFT_SHOULDER, RIGHT_SHOULDER

// Stick clicks
GamepadButton::LEFT_STICK, RIGHT_STICK

// System
GamepadButton::START, BACK, GUIDE

// Special (Xbox Elite, PS5)
GamepadButton::PADDLE1-4, TOUCHPAD
```

### Analog Stick Reading

Read raw analog stick values for advanced control:

```cpp
// In C++ InputManager
Sint16 raw = inputManager->getGamepadAxis(GamepadAxis::LEFT_X);
// Returns -32768 to 32767

// Or get normalized with deadzone
float normalized = inputManager->getGamepadAxisNormalized(
    GamepadAxis::LEFT_X, 
    true  // Apply deadzone
);
// Returns -1.0 to 1.0 with deadzone applied
```

**Available Axes:**
- `GamepadAxis::LEFT_X / LEFT_Y` - Left analog stick
- `GamepadAxis::RIGHT_X / RIGHT_Y` - Right analog stick
- `GamepadAxis::TRIGGER_LEFT / TRIGGER_RIGHT` - Analog triggers (0.0 to 1.0)

**Deadzones:**
- Analog sticks: 8000 (~24%)
- Triggers: 3000 (~9%)

---

## See Also

- `docs/LUA_API.md` - Complete Lua API reference
- `src/input/InputConstants.h` - C++ constants definition
- `cartridges/api_test/main.lua` - Input testing demo

---

**Version**: 1.1  
**Last Updated**: Phase 5.20  
**Status**: Complete - Enhanced Input
