# SPRITE EDITOR - ICON TOOLBAR EXPANSION

**Fecha:** 2025-12-20 03:18 AM  
**Objetivo:** Agregar iconos UI para las nuevas features implementadas

---

## 🎨 NUEVOS ICONOS A CREAR

### **IDs Disponibles (16-25):**

| ID | Nombre | Descripción | Hotkey | Función |
|----|--------|-------------|--------|---------|
| **16** | GRID | Grid 3x3 o hash (#) | G | Toggle grid visibility |
| **17** | FILLED_RECT | Rectángulo relleno sólido | X | Toggle filled/outline rect |
| **18** | FLIP_H | Flecha ↔ o doble flecha horizontal | H | Mirror horizontal |
| **19** | FLIP_V | Flecha ↕ o doble flecha vertical | Shift+H | Mirror vertical |
| **20** | CLEAR | X o círculo con slash | Ctrl+N | Clear canvas |
| **21** | COPY | Dos hojas superpuestas | Ctrl+C | Copy sprite |
| **22** | PASTE | Clipboard o portapapeles | Ctrl+V | Paste sprite |

---

## 🖼️ DISEÑO DE ICONOS (8x8 píxeles)

### **ID 16: GRID**
```
  # # # #
 # # # #
# # # # #
 # # # #
# # # # #
 # # # #
# # # # #
 # # # #
```
Grid simple 3x3

### **ID 17: FILLED_RECT**
```
████████
██    ██
██ ██ ██
██ ██ ██
██ ██ ██
██    ██
████████
```
Rectángulo con relleno visible

### **ID 18: FLIP_H (Horizontal)**
```
   ◄═►
  ◄═══►
 ◄═════►
◄═══════►
 ◄═════►
  ◄═══►
   ◄═►
```
Doble flecha horizontal

### **ID 19: FLIP_V (Vertical)**
```
   ▲
  ║║║
  ║║║
 ═════
  ║║║
  ║║║
   ▼
```
Doble flecha vertical

### **ID 20: CLEAR**
```
██    ██
 ██  ██
  ████
   ██
  ████
 ██  ██
██    ██
```
X grande (clear/delete)

### **ID 21: COPY**
```
 ███████
 █    ██
 █ ███ █
 █ █ █ █
███ ███
██     █
███████
```
Dos hojas superpuestas

### **ID 22: PASTE**
```
████████
█ ████ █
█ ████ █
█      █
█ ████ █
█ ████ █
████████
```
Clipboard con contenido

---

## 📐 NUEVO LAYOUT DEL TOOLBAR

### **Configuración Actual:**
```
[ Pencil ] [ Fill ] [ Line ] [ Rect ] [ Picker ]  [ Import ] [ Export ]
```
Una sola fila, 5 tools + 2 file buttons

### **Nuevo Layout (2 Filas):**
```
ROW 1: [ Pencil ] [ Fill ] [ Line ] [ Rect ] [ Picker ]
ROW 2: [ Grid ] [ Filled ] [ FlipH ] [ FlipV ] [ Clear ] [ Copy ] [ Paste ]
```

Separador entre filas: 4 píxeles

---

## 🔧 CAMBIOS TÉCNICOS

### **1. SystemSprites - Agregar nuevos iconos (IDs 16-22)**

**Archivo:** `src/ui/SystemSprites.cpp`  
**Método:** `GenerateDefaultSprites()`

Agregar generación de píxeles para IDs 16-22.

---

### **2. SpriteEditor Layout - Toolbar con 2 filas**

**Archivo:** `src/ui/SpriteEditor.h`

**Constantes a agregar:**
```cpp
// Toolbar layout (2 rows)
static constexpr int TOOLBAR_ROWS = 2;
static constexpr int TOOLBAR_ROW_HEIGHT = 20;  // 16px button + 4px spacing
static constexpr int TOOLBAR_COLS = 7;  // Max buttons per row
```

**Archivo:** `src/ui/SpriteEditor.cpp`

**Método:** `RenderToolbar()` - Modificar para 2 filas

```cpp
void SpriteEditor::RenderToolbar(AestheticLayer& renderer) {
    const int BUTTON_SIZE = 16;
    const int BUTTON_SPACING = 4;
    
    // === ROW 1: Drawing Tools ===
    int row1Y = TOOLBAR_Y;
    const int row1Icons[] = { 0, 1, 2, 3, 4 };  // Pencil, Fill, Line, Rect, Picker
    
    for (int i = 0; i < 5; i++) {
        int x = CANVAS_X + (i * (BUTTON_SIZE + BUTTON_SPACING));
        // ... draw button with icon row1Icons[i]
    }
    
    // === ROW 2: Transform & Utility Tools ===
    int row2Y = TOOLBAR_Y + TOOLBAR_ROW_HEIGHT;
    const int row2Icons[] = { 16, 17, 18, 19, 20, 21, 22 };  // Grid, Filled, FlipH, FlipV, Clear, Copy, Paste
    
    for (int i = 0; i < 7; i++) {
        int x = CANVAS_X + (i * (BUTTON_SIZE + BUTTON_SPACING));
        // ... draw button with icon row2Icons[i]
    }
}
```

**Método:** `HandleToolbarClick()` - Detectar clicks en ambas filas

```cpp
void SpriteEditor::HandleToolbarClick(int mouseX, int mouseY) {
    const int BUTTON_SIZE = 16;
    const int BUTTON_SPACING = 4;
    
    // Check ROW 1 (tools)
    int row1Y = TOOLBAR_Y;
    if (mouseY >= row1Y && mouseY < row1Y + BUTTON_SIZE) {
        // ... check tool buttons 0-4
    }
    
    // Check ROW 2 (utilities)
    int row2Y = TOOLBAR_Y + TOOLBAR_ROW_HEIGHT;
    if (mouseY >= row2Y && mouseY < row2Y + BUTTON_SIZE) {
        // Button 0: Grid toggle
        // Button 1: Filled rect toggle
        // Button 2: Flip horizontal
        // Button 3: Flip vertical
        // Button 4: Clear canvas
        // Button 5: Copy
        // Button 6: Paste
    }
}
```

---

## ⚙️ FUNCIONALIDAD DE BOTONES

### **Botón Grid (ID 16):**
```cpp
if (buttonIndex == 0) {
    showGrid = !showGrid;
    Log(showGrid ? "[Grid] Enabled" : "[Grid] Disabled");
}
```

### **Botón Filled Rect (ID 17):**
```cpp
if (buttonIndex == 1) {
    filledRectMode = !filledRectMode;
    Log(filledRectMode ? "[Rect] Filled mode" : "[Rect] Outline mode");
}
```

### **Botón Flip H (ID 18):**
```cpp
if (buttonIndex == 2) {
    SaveHistoryState();
    MirrorHorizontal();
}
```

### **Botón Flip V (ID 19):**
```cpp
if (buttonIndex == 3) {
    SaveHistoryState();
    MirrorVertical();
}
```

### **Botón Clear (ID 20):**
```cpp
if (buttonIndex == 4) {
    SaveHistoryState();
    ClearCanvas();
    Log("[Clear] Canvas cleared");
}
```

### **Botón Copy (ID 21):**
```cpp
if (buttonIndex == 5) {
    std::memcpy(clipboard, canvas, sizeof(clipboard));
    hasClipboardData = true;
    Log("[Clipboard] Sprite copied");
}
```

### **Botón Paste (ID 22):**
```cpp
if (buttonIndex == 6) {
    if (hasClipboardData) {
        SaveHistoryState();
        std::memcpy(canvas, clipboard, sizeof(canvas));
        Log("[Clipboard] Sprite pasted");
    }
}
```

---

## 🎯 INDICADORES VISUALES

### **Toggle States:**
Botones con estado on/off necesitan indicador visual:

- **Grid button:** Borde verde si `showGrid == true`
- **Filled rect button:** Borde verde si `filledRectMode == true`

```cpp
int borderColor = (isActive) ? UISystem::COLOR_GREEN : UISystem::COLOR_DARK_GRAY;
renderer.Rect(x, y, BUTTON_SIZE, BUTTON_SIZE, borderColor);
```

---

## 📋 CHECKLIST DE IMPLEMENTACIÓN

### Fase 1: Iconos
- [ ] Generar iconos 16-22 en SystemSprites.cpp
- [ ] Compilar y verificar system_sprites.png

### Fase 2: Layout
- [ ] Modificar RenderToolbar() para 2 filas
- [ ] Ajustar constantes de posicionamiento
- [ ] Compilar y verificar visualmente

### Fase 3: Interactividad
- [ ] Modificar HandleToolbarClick() para 2 filas
- [ ] Conectar cada botón a su función
- [ ] Agregar indicadores visuales de estado

### Fase 4: Testing
- [ ] Click en cada botón funciona
- [ ] Hotkeys siguen funcionando
- [ ] Estados toggle se muestran correctamente
- [ ] Import/Export no se rompieron

---

## ⏱️ ESTIMACIÓN

- **Fase 1:** 30 min (crear iconos)
- **Fase 2:** 20 min (layout)
- **Fase 3:** 30 min (interactividad)
- **Fase 4:** 10 min (testing)

**Total:** ~1.5 horas

---

**¿Proceder con la implementación?**
