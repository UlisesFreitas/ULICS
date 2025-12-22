# SPRITE EDITOR - ANÁLISIS DE ESTADO Y PLANIFICACIÓN DE BARRAS UI

**Fecha:** 2025-12-19  
**Objetivo:** Revisar estado actual del Sprite Editor y planificar adición de barras superior/inferior al estilo CodeEditor

---

## 📋 ESTADO ACTUAL DEL SPRITE EDITOR

### ✅ Implementación Completada (100%)

Revisando todos los documentos y código, el Sprite Editor tiene:

#### Funcionalidades Core (Phase 3 - COMPLETADO)
- ✅ Editor de píxeles 8x8 con zoom 16x (canvas 128x128)
- ✅ Paleta de 16 colores (layout 4x4)
- ✅ Herramientas de dibujo: Pencil, Fill, Line, Rectangle, Color Picker
- ✅ Navegación entre sprites (0-255)
- ✅ Grid de spritesheet visible (16x8 = 128 sprites)
- ✅ Guardar/cargar PNG (spritesheet.png)
- ✅ Drag support para Line y Rectangle
- ✅ Sistema de Undo/Redo (50 niveles)
- ✅ Hotkeys para herramientas (P, F, L, R, C)
- ✅ Ctrl+S para guardar
- ✅ Navegación con flechas

#### Import/Export System (SESSION_IMPORT_EXPORT - COMPLETADO)
- ✅ Botón Import con icono (SystemSprites ID 6)
- ✅ Botón Export con icono (SystemSprites ID 5)  
- ✅ File dialogs (Windows API aislado en FileDialog.h/cpp)
- ✅ Validación de PNG (debe ser 128x128)
- ✅ Import desde ubicación externa
- ✅ Export a ubicación custom con timestamp
- ✅ Hotkey Ctrl+O (Import)
- ✅ Hotkey Ctrl+E (Export)
- ✅ Export sprite individual Ctrl+Shift+E
- ✅ Recent files list (últimos 5 archivos)
- ✅ Error handling completo

### 📊 Comparación con CodeEditor

**CodeEditor tiene:**
```cpp
// BARRA SUPERIOR (10px height)
const int TITLE_H = 10;
layer.RectFill(0, 0, SCREEN_W, TITLE_H, THEME_BAR);  // COLOR_LIGHT_GRAY
layer.Print("CODE", 4, 1, THEME_BAR_TEXT);           // COLOR_BACKGROUND (black text)

// BARRA INFERIOR (10px height) 
const int STATUS_H = 10;
int statusY = EDITOR_BOTTOM;
layer.RectFill(0, statusY, SCREEN_W, STATUS_H, THEME_BAR);
layer.Print("Ln#:# filename.lua", 2, statusY + 1, THEME_BAR_TEXT);
```

**SpriteEditor actualmente tiene:**
- ❌ **NO tiene barra superior**
- ❌ **NO tiene barra inferior**
- ✅ Canvas comienza directamente en Y=20
- ✅ Toolbar en la parte inferior pero sin barra de estado

---

## 🎯 FALTA POR IMPLEMENTAR

### Según la revisión de todos los documentos:

#### 1. **De SPRITE_EDITOR_ROADMAP.md** - Future Enhancements (PENDIENTE)
Todos estos items están planeados pero NO implementados aún:

**PRIORITY 1: Quick Wins (Partially Done)**
- ✅ ~~Import/Export Hotkeys (Ctrl+O, Ctrl+E)~~ - **HECHO**
- ✅ ~~Recent Files List~~ - **HECHO**

**PRIORITY 2: UX Improvements (TODO)**
- ⏳ Custom Naming Templates (ej: `%project%_sprites_%date%.png`)
- ⏳ Drag & Drop Import (arrastrar PNG desde Explorer)

**PRIORITY 3: Advanced Features (TODO)**
- ⏳ Import Selected Sprites Only (importar solo algunos sprites)
- ⏳ Export Individual Sprites (desde context menu)
- ⏳ Multiple Spritesheet Support
- ⏳ Batch Export (exportar múltiples variaciones)

#### 2. **UI Enhancement - Barras Superior/Inferior (NUEVO)**

Esta es la **nueva funcionalidad solicitada por el usuario**:

> "Si miras CodeEditor verás que tiene una barra arriba y otra abajo en gris claro, 
> deberíamos poner dos barras iguales en el Sprite Editor así luego agregamos un 
> menú con iconos"

**Propósito:**
- Barra superior: Título, información del sprite, futuro menú de iconos
- Barra inferior: Estado, información útil (ej: sprite #, tool activa, dimensiones)
- Consistencia visual con CodeEditor
- Preparación para futuro menú global de navegación

---

## 📐 PLAN DE IMPLEMENTACIÓN: BARRAS UI

### **Fase 1: Agregar Barras Básicas** ⭐ PRIORIDAD ALTA

#### Objetivo
Agregar barras superior e inferior al Sprite Editor, iguales a las del CodeEditor.

#### Especificaciones Técnicas

**Constantes (similar a CodeEditor):**
```cpp
const int TITLE_BAR_H = 10;      // Altura barra superior
const int STATUS_BAR_H = 10;     // Altura barra inferior
const int SCREEN_W = 256;
const int SCREEN_H = 256;
```

**Layout actualizado:**
```cpp
// Y coordinates
const int TITLE_BAR_Y = 0;
const int EDITOR_TOP = TITLE_BAR_H;                    // Y=10 (antes era Y=0)
const int EDITOR_BOTTOM = SCREEN_H - STATUS_BAR_H;     // Y=246
const int STATUS_BAR_Y = EDITOR_BOTTOM;                // Y=246

// Canvas necesita bajar 10px
const int CANVAS_X = 16;      // Sin cambios
const int CANVAS_Y = 20;      // Cambiar de 20 a 30 (10px más abajo)
```

**Colores (igual que CodeEditor):**
```cpp
const int THEME_BAR = UISystem::COLOR_LIGHT_GRAY;       // Fondo de barras
const int THEME_BAR_TEXT = UISystem::COLOR_BACKGROUND;   // Texto negro
```

#### Código a agregar en `SpriteEditor::Render()`

**ANTES del Clear() actual:**
```cpp
void SpriteEditor::Render(AestheticLayer& renderer) {
    if (!isActive) return;
    
    // === Theme colors (NUEVO) ===
    const int THEME_BAR = UISystem::COLOR_LIGHT_GRAY;
    const int THEME_BAR_TEXT = UISystem::COLOR_BACKGROUND;
    const int TITLE_BAR_H = 10;
    const int STATUS_BAR_H = 10;
    const int SCREEN_W = 256;
    const int SCREEN_H = 256;
    
    // Clear screen
    renderer.Clear(UISystem::COLOR_DARK_BLUE);
    
    // === TITLE BAR (NUEVO) ===
    renderer.RectFill(0, 0, SCREEN_W, TITLE_BAR_H, THEME_BAR);
    renderer.Print("SPRITES", 4, 1, THEME_BAR_TEXT);
    
    // Sprite number in title bar (right side)
    char spriteInfo[16];
    sprintf(spriteInfo, "#%03d", currentSpriteIndex);
    int infoX = SCREEN_W - (strlen(spriteInfo) * 8) - 4;
    renderer.Print(spriteInfo, infoX, 1, THEME_BAR_TEXT);
    
    // Render all components (PICO-8 style layout) - SIN CAMBIOS
    RenderCanvas(renderer);        
    RenderPalette(renderer);       
    RenderSpritesheet(renderer);   
    RenderToolbar(renderer);       
    
    // === STATUS BAR (NUEVO) ===
    int statusY = SCREEN_H - STATUS_BAR_H;
    renderer.RectFill(0, statusY, SCREEN_W, STATUS_BAR_H, THEME_BAR);
    
    // Left side: Tool name + color
    const char* toolNames[] = { "PENCIL", "FILL", "LINE", "RECT", "PICKER" };
    char status[64];
    sprintf(status, "%s C%d", toolNames[static_cast<int>(currentTool)], selectedColor);
    renderer.Print(status, 2, statusY + 1, THEME_BAR_TEXT);
    
    // Right side: Undo stack size (debug info)
    char undoInfo[16];
    sprintf(undoInfo, "U:%d", static_cast<int>(undoStack.size()));
    int undoX = SCREEN_W - (strlen(undoInfo) * 8) - 2;
    renderer.Print(undoInfo, undoX, statusY + 1, THEME_BAR_TEXT);
}
```

#### Ajustes de posición

**En SpriteEditor.h actualizar constantes:**
```cpp
// OLD:
static constexpr int CANVAS_Y = 20;       
static constexpr int PALETTE_Y = 20;      
static constexpr int SHEET_Y = 160;       
static constexpr int TOOLBAR_Y = 235;     

// NEW:
static constexpr int CANVAS_Y = 30;        // +10px por title bar
static constexpr int PALETTE_Y = 30;       // +10px por title bar
static constexpr int SHEET_Y = 170;        // +10px por title bar
static constexpr int TOOLBAR_Y = 230;      // -5px para hacer espacio a status bar
```

#### Archivos a modificar
- `src/ui/SpriteEditor.h` - Actualizar constantes
- `src/ui/SpriteEditor.cpp` - Render() method

#### Estimación
- **Tiempo:** 30 minutos
- **Complejidad:** BAJA (es solo UI, no lógica)
- **Testing:** Visual, verificar que nada se solape

---

### **Fase 2: Preparar para Menú de Iconos** ⏳ FUTURO

**Objetivo:** Dejar espacio/estructura para futuro menú global

**Ideas para barra superior:**
```
┌─────────────────────────────────────────────────┐
│ [≡] SPRITES    #042      [◀] [▶] [📁] [💾] [?]  │
│  ↑   ↑         ↑          ↑----- Icons ------↑  │
│ menú título  número    navegación y acciones    │
└─────────────────────────────────────────────────┘
```

**Posibles iconos:**
- `[≡]` - Menú hamburguesa (toggle global menu)
- `[◀]` `[▶]` - Prev/Next sprite
- `[📁]` - Import
- `[💾]` - Save
- `[?]` - Help/Info

**Esta fase NO se implementa ahora, solo se planifica.**

---

## 📊 RESUMEN DE ACCIONES

### ✅ Ya está hecho (NO requiere acción)
1. ✅ Todas las funcionalidades del Phase 3
2. ✅ Sistema completo de Import/Export
3. ✅ Undo/Redo system
4. ✅ Recent files
5. ✅ Hotkeys completos
6. ✅ Validación de PNG
7. ✅ Export de sprites individuales

### 🔧 Pendiente de implementar (del Roadmap)
1. ⏳ Custom Naming Templates
2. ⏳ Drag & Drop Import
3. ⏳ Import Selected Sprites Only
4. ⏳ Context menu para sprite grid
5. ⏳ Multiple Spritesheet Support
6. ⏳ Batch Export

### ⭐ **NUEVA TAREA PRIORITARIA**
**Agregar barras superior e inferior** (este documento)
- Estimación: 30 min
- Archivos: SpriteEditor.h, SpriteEditor.cpp
- Sin dependencias
- Listo para implementar **AHORA**

---

## 🚀 SIGUIENTE PASO RECOMENDADO

**Implementar Fase 1: Barras UI básicas**

### Why now?
1. **Fácil** - Solo cambios visuales, sin lógica compleja
2. **Rápido** - 30 minutos
3. **Fundación** - Prepara para futuro menú global
4. **Consistencia** - Iguala con CodeEditor
5. **Sin riesgos** - No afecta funcionalidad existente

### Después de esto:
- Considerar implementar **Drag & Drop** (mejor UX)
- O **Custom Naming Templates** (más profesional)
- O dejar listo para integración de **menú global** (si es prioridad del proyecto)

---

**Autor:** Análisis automático basado en documentación y código  
**Última actualización:** 2025-12-19 22:49
