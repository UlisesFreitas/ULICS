# Menu System Design - ULICS

**Fecha:** 2025-12-22  
**Estado:** Diseño  
**Fase:** PHASE 1 - Análisis y Diseño

---

## 📊 Estado Actual del Sistema

### EngineState (Estado del Motor)

```cpp
enum class EngineState {
    BOOT,                // Arranque inicial
    MENU,                // Menú de selección de cartridges
    LOADING_CARTRIDGE,   // Cargando un cartridge
    RUNNING_CARTRIDGE,   // Cartridge corriendo
    ERROR                // Estado de error
};
```

**Transiciones actuales:**
```
BOOT → MENU (automático)
MENU → LOADING_CARTRIDGE (seleccionar cart)
LOADING_CARTRIDGE → RUNNING_CARTRIDGE (éxito)
LOADING_CARTRIDGE → ERROR (fallo)
* → ERROR (cualquier error fatal)
```

### EngineMode (Modo dentro de RUNNING_CARTRIDGE)

```cpp
enum class EngineMode {
    GAME,          // Jugando (default)
    CODE_EDITOR,   // F1 - Editando código
    SPRITE_EDITOR, // F2 - Editando sprites
    MAP_EDITOR,    // F3 - Editando mapa (futuro)
    SFX_EDITOR,    // F4 - Editando SFX (futuro)
    MUSIC_EDITOR   // F5 - Editando música (futuro)
};
```

**Transiciones actuales:**
```
GAME ↔ CODE_EDITOR (F1)
GAME ↔ SPRITE_EDITOR (F2)
CODE_EDITOR ↔ SPRITE_EDITOR (F1/F2)
EDITOR ← ESC → GAME
```

### Problemas Identificados

1. ⚠️ **No hay Pause Menu:** ESC en GAME no hace nada
2. ⚠️ **No hay retorno a MENU:** Una vez en cartridge, solo cerrar app
3. ⚠️ **MENU actual es script Lua:** Difícil de controlar desde C++
4. ⚠️ **No hay Settings persistentes**
5. ⚠️ **Transiciones bruscas:** Sin fade, sin animación

---

## 🎯 Diseño Propuesto

### Nuevos Estados

```cpp
enum class EngineState {
    BOOT,                // Arranque inicial
    MAIN_MENU,          // ← NUEVO: Menú principal nativo C++
    PAUSE_MENU,         // ← NUEVO: Menú de pausa
    SETTINGS_MENU,      // ← NUEVO: Configuración
    LOADING_CARTRIDGE,   
    RUNNING_CARTRIDGE,   
    ERROR                
};
```

### Flujo de Estados Completo

```
                    [BOOT]
                      ↓
                 [MAIN_MENU] ←────────────────┐
                   ↓     ↑                    │
        SELECT     │     │  QUIT TO MENU      │
                   ↓     │                    │
            [LOADING_CARTRIDGE]               │
                   ↓                          │
            [RUNNING_CARTRIDGE]               │
              (EngineMode activo)             │
                   ↓                          │
              ESC pressed                     │
                   ↓                          │
              [PAUSE_MENU] ───────────────────┘
                   ↓     ↑
            RESUME │     │ SETTINGS
                   ↓     │
              [RUNNING_CARTRIDGE]
                         │
                    [SETTINGS_MENU]
```

---

## 🎨 Diseños de UI

### Main Menu

```
┌──────────────────────────────────────┐
│                                      │
│         ██╗   ██╗██╗     ██╗ ██████╗│
│         ██║   ██║██║     ██║██╔════╝│
│         ██║   ██║██║     ██║██║     │
│         ██║   ██║██║     ██║██║     │
│         ╚██████╔╝███████╗██║╚██████╗│
│          ╚═════╝ ╚══════╝╚═╝ ╚═════╝│
│                                      │
│              Fantasy Console         │
│                                      │
│       ► SELECT CARTRIDGE             │
│         SETTINGS                     │
│         QUIT                         │
│                                      │
│                                      │
│       [↑↓] Navigate  [ENTER] Select  │
└──────────────────────────────────────┘
```

**Colores:**
- Background: `SystemColors::DARK_BLUE`
- Title: `SystemColors::WHITE` / `SystemColors::CYAN`
- Selected item: `SystemColors::YELLOW` + highlight
- Normal items: `SystemColors::LIGHT_GRAY`

### Cartridge Selection

```
┌──────────────────────────────────────┐
│  SELECT CARTRIDGE                    │
├──────────────────────────────────────┤
│                                      │
│  ► api_test                          │
│    audio_test                        │
│    my_game                           │
│    platformer_demo                   │
│    puzzle_game                       │
│                                      │
│  [5 cartridges found]                │
│                                      │
│  [↑↓] Navigate  [ENTER] Load         │
│  [ESC] Back                          │
└──────────────────────────────────────┘
```

### Pause Menu

```
┌──────────────────────────────────────┐
│              PAUSED                  │
├──────────────────────────────────────┤
│                                      │
│       ► RESUME                       │
│         RESTART                      │
│         SETTINGS                     │
│         QUIT TO MENU                 │
│                                      │
│                                      │
│  [↑↓] Navigate  [ENTER] Select       │
│  [ESC] Resume                        │
└──────────────────────────────────────┘
```

**Comportamiento:**
- Background: Game screen oscurecido (overlay)
- Semi-transparente (opcional)
- Pausa completa del juego (no más _update())

### Settings Menu

```
┌──────────────────────────────────────┐
│  SETTINGS                            │
├──────────────────────────────────────┤
│                                      │
│  ► VOLUME         [■■■■■□□□] 70%     │
│    FULLSCREEN     [ OFF ]            │
│    DEBUG MODE     [ ON  ]            │
│    VSYNC          [ ON  ]            │
│                                      │
│    RESET TO DEFAULTS                 │
│                                      │
│  [↑↓] Navigate  [←→] Adjust          │
│  [ENTER] Toggle  [ESC] Back          │
└──────────────────────────────────────┘
```

---

## 🔧 Arquitectura Técnica

### MenuSystem Class

```cpp
class MenuSystem {
public:
    struct MenuItem {
        std::string text;
        std::function<void()> action;  // Callback al seleccionar
        bool enabled = true;
        bool isSlider = false;         // Para settings
        int* valuePtr = nullptr;       // Puntero a valor para sliders
        int minValue = 0;
        int maxValue = 100;
    };
    
    MenuSystem();
    ~MenuSystem();
    
    // Setup
    void AddItem(const std::string& text, std::function<void()> action);
    void AddSlider(const std::string& text, int* valuePtr, int min, int max);
    void Clear();
    
    // Input
    void Update(InputManager& input);
    
    // Rendering
    void Render(AestheticLayer& layer);
    
    // State
    void SetTitle(const std::string& title);
    int GetSelectedIndex() const;
    
private:
    std::string title;
    std::vector<MenuItem> items;
    int selectedIndex;
    bool visible;
    
    // Helper para rendering
    void RenderMenuItem(AestheticLayer& layer, const MenuItem& item, 
                       int x, int y, bool selected);
};
```

### Settings Class

```cpp
class Settings {
public:
    Settings();
    ~Settings();
    
    // Load/Save
    bool Load();  // From AppData/Roaming/ULICS/settings.json
    bool Save();
    
    // Getters/Setters
    int GetVolume() const { return volume; }
    void SetVolume(int vol) { volume = clamp(vol, 0, 100); }
    
    bool IsFullscreen() const { return fullscreen; }
    void SetFullscreen(bool fs) { fullscreen = fs; }
    
    bool IsDebugMode() const { return debugMode; }
    void SetDebugMode(bool dm) { debugMode = dm; }
    
    bool IsVSync() const { return vsync; }
    void SetVSync(bool vs) { vsync = vs; }
    
    // Defaults
    void ResetToDefaults();
    
private:
    int volume;
    bool fullscreen;
    bool debugMode;
    bool vsync;
    
    std::string GetSettingsPath() const;
};
```

### Estado Management en Engine

```cpp
// Engine.h additions
private:
    std::unique_ptr<MenuSystem> mainMenu;
    std::unique_ptr<MenuSystem> pauseMenu;
    std::unique_ptr<MenuSystem> settingsMenu;
    std::unique_ptr<Settings> settings;
    
    void InitializeMenus();
    void RenderMainMenu();
    void RenderPauseMenu();
    void RenderSettingsMenu();
```

---

## 🎮 Input Mapping

### Navegación de Menús

```
UP / W           → Selección anterior
DOWN / S         → Selección siguiente
ENTER / SPACE    → Confirmar selección
ESC              → Volver/Cancelar
LEFT / A         → Ajustar valor (sliders)
RIGHT / D        → Ajustar valor (sliders)
```

### Teclas Especiales

```
F1-F5   → Editores (solo en RUNNING_CARTRIDGE)
ESC     → Contexto-dependiente:
          - GAME → PAUSE_MENU
          - PAUSE_MENU → RESUME
          - MAIN_MENU → (nada)
          - SETTINGS → volver al menú anterior
```

---

## 🔄 State Transitions

### Detalle de Transiciones

```cpp
// Pseudo-código de transiciones

void Engine::HandleStateTransitions() {
    switch (currentState) {
        case EngineState::MAIN_MENU:
            if (cartridgeSelected) {
                SetState(EngineState::LOADING_CARTRIDGE);
            }
            else if (settingsRequested) {
                SetState(EngineState::SETTINGS_MENU);
            }
            else if (quitRequested) {
                isRunning = false;
            }
            break;
            
        case EngineState::RUNNING_CARTRIDGE:
            if (currentMode == EngineMode::GAME && escPressed) {
                SetState(EngineState::PAUSE_MENU);
                // NO destruir activeGame
            }
            break;
            
        case EngineState::PAUSE_MENU:
            if (resumeSelected || escPressed) {
                SetState(EngineState::RUNNING_CARTRIDGE);
            }
            else if (restartSelected) {
                ReloadCurrentCartridge();
                SetState(EngineState::RUNNING_CARTRIDGE);
            }
            else if (settingsSelected) {
                SetState(EngineState::SETTINGS_MENU);
            }
            else if (quitToMenuSelected) {
                UnloadCartridge();
                SetState(EngineState::MAIN_MENU);
            }
            break;
    }
}
```

---

## 📁 Archivos a Crear/Modificar

### Nuevos

1. **`src/ui/MenuSystem.h`** - Clase base de menús
2. **`src/ui/MenuSystem.cpp`** - Implementación
3. **`src/core/Settings.h`** - Gestión de configuración
4. **`src/core/Settings.cpp`** - Implementación

### Modificar

5. **`src/core/Engine.h`** 
   - Agregar `PAUSE_MENU`, `SETTINGS_MENU` a EngineState
   - Agregar punteros a MenuSystem y Settings
   
6. **`src/core/Engine.cpp`**
   - `InitializeMenus()` - Setup de menús
   - `Run()` - Manejo de estados de menú
   - Render para cada estado de menú

7. **`CMakeLists.txt`**
   - Agregar MenuSystem.cpp y Settings.cpp

---

## ✅ Checklist de Implementación

### PHASE 1: Análisis ✅
- [x] Mapear estados actuales
- [x] Diseñar nuevos estados
- [x] Diseñar UIs de menús
- [x] Definir flujos de navegación

### PHASE 2: Sistema de Estados
- [ ] Extender EngineState
- [ ] Mejorar SetState() con validaciones
- [ ] Testing de transiciones

### PHASE 3: MenuSystem Base
- [ ] Crear MenuSystem.h/cpp
- [ ] Implementar MenuItem
- [ ] Rendering con SystemColors
- [ ] Input handling

### PHASE 4: Main Menu
- [ ] Diseñar UI
- [ ] Implementar navegación de cartridges
- [ ] Transición a load

### PHASE 5: Pause Menu
- [ ] Detectar ESC en GAME
- [ ] Diseñar Pause UI
- [ ] Implementar opciones
- [ ] Preservar estado del juego

### PHASE 6: Settings
- [ ] Definir settings
- [ ] Implementar Settings class
- [ ] UI de settings

### PHASE 7: Polish
- [ ] Fade transitions
- [ ] Sonidos (opcional)
- [ ] Animaciones (opcional)

---

**Status:** Diseño completo  
**Próximo paso:** Implementación PHASE 2
