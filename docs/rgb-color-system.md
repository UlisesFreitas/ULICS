# Sistema de Colores RGB Fijo - ULICS

**Fecha:** 2025-12-21  
**Sesión:** Refactor completo del sistema de colores  
**Duración:** ~2.5 horas  
**Status:** ✅ COMPLETADO

---

## 📋 Tabla de Contenidos

1. [Problema Original](#problema-original)
2. [Solución Implementada](#solución-implementada)
3. [Arquitectura del Sistema](#arquitectura-del-sistema)
4. [Cambios Realizados](#cambios-realizados)
5. [API Reference](#api-reference)
6. [Guía de Uso](#guía-de-uso)
7. [Sistema de Paletas](#sistema-de-paletas)
8. [Migraciones Futuras](#migraciones-futuras)

---

## 🔴 Problema Original

### Descripción del Bug

Al importar una **paleta personalizada** (.pal) en el Sprite Editor, **toda la UI de la aplicación** cambiaba de color, no solo los sprites editables.

**Comportamiento observado:**
- Importar `alternative-palette.pal` → La barra superior se volvía verde
- Importar `mine.pal` → Todo el texto se volvía diferente
- Los iconos de herramientas cambiaban de color
- El Code Editor cambiaba de color
- La UI era **ilegible** con ciertas paletas

**Causa raíz:**
- Toda la UI usaba colores **indexados** de la paleta (índices 0-31)
- `AestheticLayer` tenía una única paleta compartida
- Cambiar la paleta afectaba **todo** lo que se renderizaba

### Objetivo de la Solución

**Permitir importar paletas de 32 colores completos** para sprites **SIN afectar la UI** de la aplicación.

---

## ✅ Solución Implementada

### Concepto Core

**Separar completamente** el renderizado en dos sistemas:

1. **UI (interfaz)** → Colores RGB fijos, nunca afectados por paletas
2. **Contenido editable (sprites)** → Usa paleta indexada editable

### Arquitectura de Dos Capas

```
┌─────────────────────────────────────────┐
│         CAPA DE UI (RGB FIJO)           │
│  - Barras, botones, texto, bordes       │
│  - Usa SystemColors (constantes RGB)    │
│  - Dibuja con PrintRGB(), RectRGB()     │
│  - Marca framebuffer con 255            │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│    CAPA DE CONTENIDO (PALETA INDEXADA)  │
│  - Pixels de sprites editables          │
│  - Usa palette editable (0-31)          │
│  - Dibuja con RectFill() indexado       │
│  - Framebuffer normal (0-31)            │
└─────────────────────────────────────────┘
```

---

## 🏗️ Arquitectura del Sistema

### 1. SystemColors.h - Colores Fijos RGB

**Ubicación:** `src/ui/SystemColors.h`

```cpp
namespace SystemColors {
    // 16 Core PICO-8 colors
    constexpr SDL_Color BLACK = {0, 0, 0, 255};
    constexpr SDL_Color DARK_BLUE = {29, 43, 83, 255};
    constexpr SDL_Color DARK_PURPLE = {126, 37, 83, 255};
    // ... 13 more core colors
    
    // 16 Extended TIC-80 colors
    constexpr SDL_Color BROWN = {164, 100, 34, 255};
    constexpr SDL_Color DARK_GRAY = {95, 87, 79, 255};
    // ... 14 more extended colors
    
    // UI Semantic Aliases
    constexpr SDL_Color UI_BACKGROUND = DARK_BLUE;
    constexpr SDL_Color UI_TEXT_PRIMARY = WHITE;
    constexpr SDL_Color UI_BORDER = LIGHT_GRAY;
    // ... more aliases
    
    // Helper for legacy code
    SDL_Color GetFixedColor(int index);
}
```

**Características:**
- ✅ 32 colores RGB fijos (16 PICO-8 + 16 TIC-80)
- ✅ Aliases semánticos para UI
- ✅ Constexpr para optimización
- ✅ Compatible con SDL_Color

### 2. AestheticLayer - Funciones RGB

**Ubicación:** `src/rendering/AestheticLayer.h/cpp`

#### Nuevas Funciones RGB

```cpp
// Dibujar texto con color RGB fijo
void PrintRGB(const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b);

// Dibujar línea con color RGB fijo
void LineRGB(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b);

// Dibujar borde de rectángulo con color RGB fijo
void RectRGB(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b);

// (RectFillRGB ya existía desde antes)
void RectFillRGB(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b);
```

#### Implementación Técnica

**Mecanismo de protección contra sobrescritura:**

1. Las funciones RGB escriben directamente a `pixelBuffer` (RGB directo)
2. Marcan los píxeles correspondientes en `framebuffer` con el valor especial `255`
3. En `Present()`, se skippean los píxeles marcados con `255`

```cpp
void AestheticLayer::RectFillRGB(int x, int y, int width, int height, 
                                 uint8_t r, uint8_t g, uint8_t b) {
    for (int py = 0; py < height; py++) {
        for (int px = 0; px < width; px++) {
            int screenX = x + px;
            int screenY = y + py;
            
            if (screenX >= 0 && screenX < screenWidth && 
                screenY >= 0 && screenY < screenHeight) {
                int pixelIndex = (screenY * screenWidth + screenX) * 4;
                
                // Write RGB directly
                pixelBuffer[pixelIndex + 0] = r;
                pixelBuffer[pixelIndex + 1] = g;
                pixelBuffer[pixelIndex + 2] = b;
                pixelBuffer[pixelIndex + 3] = 255;
                
                // Mark as RGB in framebuffer
                framebuffer[screenY * screenWidth + screenX] = 255;
            }
        }
    }
}

void AestheticLayer::Present() {
    for (int i = 0; i < screenWidth * screenHeight; i++) {
        // Skip RGB pixels (marked with 255)
        if (framebuffer[i] == 255) continue;
        
        // Convert indexed color to RGB
        uint8_t colorIndex = framebuffer[i];
        SDL_Color color = currentPalette[colorIndex];
        
        pixelBuffer[i * 4 + 0] = color.r;
        pixelBuffer[i * 4 + 1] = color.g;
        pixelBuffer[i * 4 + 2] = color.b;
        pixelBuffer[i * 4 + 3] = 255;
    }
    // ... SDL texture update
}
```

### 3. Sistema de Paletas

**Funcionalidad actual:**

```cpp
// Cargar paleta completa (32 colores)
void LoadPalette(const std::vector<SDL_Color>& palette);

// Obtener color de la paleta
SDL_Color GetPaletteColor(int index) const;
```

**Formato .pal:**
- Binario: 32 colores × 3 bytes RGB = 96 bytes
- Sin header, solo raw RGB

**Ubicación de paletas:**
- Por cartridge: `./cartridges/<cart_name>/palette.pal`
- Auto-load al cargar cartridge (Engine.cpp)
- Auto-save al importar paleta (SpriteEditor.cpp)

---

## 📝 Cambios Realizados

### Archivos Nuevos

1. **`src/ui/SystemColors.h`** (NEW)
   - Sistema de colores RGB fijos
   - 32 colores + aliases semánticos

### Archivos Modificados

#### Core Rendering

2. **`src/rendering/AestheticLayer.h`**
   - Agregadas declaraciones: `PrintRGB()`, `LineRGB()`, `RectRGB()`

3. **`src/rendering/AestheticLayer.cpp`**
   - Implementación de funciones RGB
   - Modificado `Present()` para skipear píxeles RGB (255)

#### UI - Sprite Editor

4. **`src/ui/SpriteEditor.cpp`**
   - `#include "ui/SystemColors.h"`
   - `RenderCanvas()` → RGB
   - `RenderSpritesheet()` → RGB
   - `RenderToolbar()` → RGB
   - `RenderUtilityBar()` → RGB
   - `RenderPalette()` → RGB (UI elements only)
   - `RenderCursorHighlight()` → RGB
   - `RenderDragPreview()` → RGB
   - Top bar y status bar → RGB
   - `ImportPalette()` → auto-save con `SaveCartridgePalette()`
   - Permitir dibujar color 0 (eliminado skip)

#### UI - Code Editor

5. **`src/ui/CodeEditor.cpp`**
   - `#include "ui/SystemColors.h"`
   - Toda la función `Render()` → RGB
   - Title bar, status bar, line numbers → RGB
   - Selection highlight, cursor → RGB

6. **`src/ui/FileExplorer.cpp`**
   - `#include "ui/SystemColors.h"`
   - Background, borders, text → RGB
   - Selection highlights → RGB
   - Scroll indicators → RGB

#### Syntax Highlighting

7. **`src/ui/LuaSyntax.h`**
   - `#include "ui/SystemColors.h"`
   - `GetColorForToken()` → retorna `SDL_Color` en vez de `int`
   - Todos los colores → SystemColors

8. **`src/editor/rendering/SyntaxHighlighter.cpp`**
   - `RenderLine()` → usa `PrintRGB()` con SystemColors
   - Keywords, strings, numbers, comments → RGB fijo

#### System UI

9. **`src/ui/SystemSprites.cpp`**
   - `DrawSprite()` → usa `SystemColors::GetFixedColor()`
   - Iconos de herramientas → RGB fijo

10. **`src/ui/UISystem.h`**
    - **ELIMINADOS** todos los `COLOR_*` constantes
    - Solo quedan funciones utilitarias

11. **`src/ui/UISystem.cpp`**
    - Stubeado temporalmente (no usado actualmente)
    - TODOs para migración futura a RGB

#### Engine Core

12. **`src/core/Engine.cpp`**
    - `LoadCartridge()` → auto-load `palette.pal` si existe
    - Carga automática de paletas al iniciar cartridge

---

## 🎨 API Reference

### Uso Básico - Dibujar UI

```cpp
// Include SystemColors
#include "ui/SystemColors.h"

// Dibujar fondo
renderer.RectFillRGB(x, y, width, height,
                     SystemColors::DARK_BLUE.r,
                     SystemColors::DARK_BLUE.g,
                     SystemColors::DARK_BLUE.b);

// Dibujar borde
renderer.RectRGB(x, y, width, height,
                 SystemColors::LIGHT_GRAY.r,
                 SystemColors::LIGHT_GRAY.g,
                 SystemColors::LIGHT_GRAY.b);

// Dibujar texto
renderer.PrintRGB("Hello", x, y,
                  SystemColors::WHITE.r,
                  SystemColors::WHITE.g,
                  SystemColors::WHITE.b);

// Dibujar línea
renderer.LineRGB(x0, y0, x1, y1,
                 SystemColors::GRAY.r,
                 SystemColors::GRAY.g,
                 SystemColors::GRAY.b);
```

### Colores Disponibles

**Core PICO-8 (0-15):**
```
BLACK, DARK_BLUE, DARK_PURPLE, DARK_GREEN,
BROWN, DARK_GRAY, LIGHT_GRAY, WHITE,
RED, ORANGE, YELLOW, GREEN,
BLUE, INDIGO, PINK, PEACH
```

**Extended TIC-80 (16-31):**
```
MAROON, NAVY, PURPLE, OLIVE,
TEAL, SILVER, GRAY, LIME,
AQUA, FUCHSIA, LAVENDER, GOLD,
CORAL, SALMON, HOT_PINK, CYAN
```

**Aliases Semánticos:**
```
UI_BACKGROUND, UI_TEXT_PRIMARY, UI_TEXT_SECONDARY,
UI_BORDER, UI_BORDER_LIGHT, UI_HIGHLIGHT,
UI_BUTTON_BG, UI_BUTTON_HOVER, UI_BUTTON_ACTIVE
```

### Sistema de Paletas

```cpp
// Cargar paleta desde archivo .pal
void ImportPalette() {
    std::string filename = FileDialog::OpenFile("*.pal");
    std::ifstream file(filename, std::ios::binary);
    
    std::vector<SDL_Color> palette;
    for (int i = 0; i < 32; i++) {
        uint8_t r, g, b;
        file.read(&r, 1);
        file.read(&g, 1);
        file.read(&b, 1);
        palette.push_back({r, g, b, 255});
    }
    
    aestheticLayer->LoadPalette(palette);
    SaveCartridgePalette(); // Auto-save
}

// Exportar paleta
void ExportPalette() {
    std::string filename = FileDialog::SaveFile("*.pal");
    std::ofstream file(filename, std::ios::binary);
    
    for (int i = 0; i < 32; i++) {
        SDL_Color color = aestheticLayer->GetPaletteColor(i);
        file.write((char*)&color.r, 1);
        file.write((char*)&color.g, 1);
        file.write((char*)&color.b, 1);
    }
}
```

---

## 📚 Guía de Uso

### Para Nuevos Editores

Si vas a crear un nuevo editor (Map Editor, Sound Editor, etc.):

**1. Include SystemColors**
```cpp
#include "ui/SystemColors.h"
```

**2. Para UI, usa SIEMPRE RGB**
```cpp
// ✅ CORRECTO - UI con RGB fijo
renderer.RectFillRGB(x, y, w, h,
                     SystemColors::UI_BACKGROUND.r,
                     SystemColors::UI_BACKGROUND.g,
                     SystemColors::UI_BACKGROUND.b);

// ❌ INCORRECTO - UI con índice de paleta
renderer.RectFill(x, y, w, h, 1); // Cambiará con paleta!
```

**3. Para contenido editable, usa paleta indexada**
```cpp
// ✅ CORRECTO - Pixel de sprite/mapa editable
renderer.RectFill(x, y, 1, 1, colorIndex); // 0-31
```

### Regla de Oro

> **UI = RGB fixed**  
> **User Content = Palette indexed**

---

## 🎯 Sistema de Paletas

### Flujo de Paletas por Cartridge

```
1. Usuario carga cartridge "my_game"
   ↓
2. Engine.cpp LoadCartridge() busca:
   ./cartridges/my_game/palette.pal
   ↓
3a. Si existe → carga automáticamente
3b. Si NO existe → usa paleta default PICO-8
   ↓
4. Usuario puede ir a Sprite Editor:
   - Ver sprites con paleta actual
   - Importar nueva paleta (.pal)
   - Editar colores individuales
   ↓
5. Al importar o editar:
   - Auto-save a palette.pal del cartridge
   ↓
6. Próxima carga:
   - Paleta personalizada se carga automáticamente
```

### Ubicaciones de Archivos

```
./cartridges/
  ├── my_game/
  │   ├── main.lua
  │   ├── config.json
  │   ├── spritesheet.png
  │   └── palette.pal          ← Paleta del cartridge
  │
  └── another_game/
      ├── main.lua
      └── palette.pal          ← Paleta independiente
```

### Formato .pal Binary

```
Offset  Size  Content
------  ----  -------
0x00    3     Color 0 (R,G,B)
0x03    3     Color 1 (R,G,B)
...
0x5A    3     Color 30 (R,G,B)
0x5D    3     Color 31 (R,G,B)
------
Total: 96 bytes (32 × 3)
```

---

## 🚀 Migraciones Futuras

### Componentes Pendientes

Si creas nuevos componentes UI, recuerda migrar a RGB:

1. **Map Editor** (cuando se implemente)
   - UI → SystemColors RGB
   - Tiles → paleta indexada

2. **Sound Editor** (cuando se implemente)
   - UI → SystemColors RGB
   - Visualizaciones → RGB o indexado según caso

3. **Music Editor** (cuando se implemente)
   - UI → SystemColors RGB

4. **Dialog Systems**
   - Usar SystemColors RGB

5. **HUD Custom del Usuario**
   - Puede usar **paleta indexada** (es contenido del juego)

### Checklist para Nuevos Componentes

```
[ ] Include "ui/SystemColors.h"
[ ] Botones/barras → RectFillRGB + SystemColors
[ ] Texto → PrintRGB + SystemColors
[ ] Bordes → RectRGB + SystemColors
[ ] Líneas → LineRGB + SystemColors
[ ] Contenido editable → indexado (si aplica)
[ ] Testing con paleta importada
```

---

## 🐛 Bugs Resueltos

### Color 0 no dibujaba

**Problema:** Había un check `if (colorIndex != 0)` que saltaba el color 0.

**Solución:** Eliminado el check. Ahora TODOS los 32 colores son dibujables.

```cpp
// ❌ ANTES
if (colorIndex != 0) {
    renderer.RectFill(x, y, 1, 1, colorIndex);
}

// ✅ AHORA
renderer.RectFill(x, y, 1, 1, colorIndex); // 0-31 todos válidos
```

### Paleta no persistía entre sesiones

**Problema:** `ImportPalette()` solo cargaba en memoria, no guardaba.

**Solución:** Auto-save a `palette.pal` al importar.

```cpp
void SpriteEditor::ImportPalette() {
    // ... cargar paleta
    aestheticLayer->LoadPalette(newPalette);
    SaveCartridgePalette(); // ← AUTO-SAVE agregado
}
```

### Paleta no se cargaba al inicio

**Problema:** Paleta solo se cargaba al entrar al Sprite Editor.

**Solución:** Cargar en `Engine::LoadCartridge()`.

```cpp
bool Engine::LoadCartridge(const std::string& cartridgePath) {
    // ... configurar aesthetic layer
    
    // Load custom palette.pal if exists
    std::filesystem::path palettePath = cartPath / "palette.pal";
    if (std::filesystem::exists(palettePath)) {
        // Load and apply palette
        aestheticLayer->LoadPalette(loadedPalette);
    }
}
```

---

## 📊 Estadísticas Finales

- **Archivos modificados:** 12
- **Archivos nuevos:** 1 (SystemColors.h)
- **Líneas de código agregadas:** ~500
- **Líneas de código modificadas:** ~300
- **Bugs resueltos:** 4
- **Funciones nuevas RGB:** 3 (PrintRGB, LineRGB, RectRGB)
- **Colores SystemColors:** 32 + 9 aliases
- **Tiempo de desarrollo:** ~2.5 horas
- **Tests exitosos:** ✅ Import/export paletas, UI inmune

---

## ✅ Checklist de Validación

- [x] Importar paleta → UI no cambia
- [x] Code Editor → colores fijos
- [x] Sprite Editor → colores fijos en UI
- [x] Syntax Highlighting → colores fijos
- [x] File Explorer → colores fijos
- [x] Todos los 32 colores dibujables
- [x] Auto-save palette.pal funciona
- [x] Auto-load palette.pal funciona
- [x] Paletas independientes por cartridge

---

## 🎓 Lecciones Aprendidas

1. **Separación de concerns crítica:** UI vs Contenido DEBE estar separado
2. **Framebuffer dual approach:** Marcar píxeles RGB con valor especial (255) funciona bien
3. **Constexpr para colores:** Optimización del compilador + type safety
4. **Auto-save/load:** Mejor UX que manual
5. **Semantic aliases:** `UI_BACKGROUND` es más claro que `DARK_BLUE`

---

## 📞 Para Soporte

Si encuentras problemas con el sistema de colores:

1. Verifica que uses `SystemColors.h`
2. Verifica que uses funciones `*RGB()` para UI
3. Verifica que palette.pal exista (si esperas colores custom)
4. Revisa console logs durante `LoadCartridge()`

**Autor:** Antigravity AI  
**Fecha:** 2025-12-21  
**Versión:** 1.0
