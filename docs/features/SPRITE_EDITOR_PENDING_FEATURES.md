# SPRITE EDITOR - FEATURES PENDIENTES

**Fecha:** 2025-12-20 05:05 AM  
**Estado:** Documentación para implementación futura

---

## 🎨 **FEATURE 1: Ampliar Paleta de 16 a 32 Colores**

### **Situación Actual:**
- Paleta: **16 colores** (PICO-8 style)
- Layout: **4x4 grid** (16 boxes de 12x12 px)
- Posición: Derecha del editor (X=176, Y=30)

### **Objetivo:**
- Paleta: **32 colores** (TIC-80 style)
- Layout: **8x4 grid** o **4x8 grid**
- Mantener compatibilidad con sprites existentes (primeros 16 colores)

---

### **Diseño Propuesto - Opción A: 8x4 (Horizontal)**

```
┌────────────────────────────────────┐
│ 0  1  2  3  4  5  6  7  8  9 10 11│  Row 1
│12 13 14 15 16 17 18 19 20 21 22 23│  Row 2
│24 25 26 27 28 29 30 31 -- -- -- --│  Row 3-4 (extras)
└────────────────────────────────────┘
```

**Dimensiones:**
- 8 columnas × 4 filas = 32 colores
- Box size: 12x12 px (mantener)
- Total width: 8 * 12 = 96px
- Total height: 4 * 12 = 48px

**Pros:** Compacta verticalmente  
**Contras:** Muy ancha, puede interferir con layout

---

### **Diseño Propuesto - Opción B: 4x8 (Vertical) ✅ RECOMENDADO**

```
┌──────────┐
│ 0  1  2  3│  Row 1
│ 4  5  6  7│  Row 2
│ 8  9 10 11│  Row 3
│12 13 14 15│  Row 4 ← PICO-8 colors (actual)
├──────────┤
│16 17 18 19│  Row 5 ← Extended colors
│20 21 22 23│  Row 6
│24 25 26 27│  Row 7
│28 29 30 31│  Row 8
└──────────┘
```

**Dimensiones:**
- 4 columnas × 8 filas = 32 colores
- Box size: 12x12 px (mantener)
- Total width: 4 * 12 = 48px (igual que ahora)
- Total height: 8 * 12 = 96px (doble altura)

**Pros:** Mismo ancho, compatible con layout actual  
**Contras:** Ocupa más altura vertical

---

### **Paleta de 32 Colores Propuesta (TIC-80 Compatible)**

```cpp
// First 16 colors (PICO-8 compatible - MANTENER)
static constexpr uint32_t PALETTE_16[16] = {
    0xFF000000,  // 0: Black
    0xFF1D2B53,  // 1: Dark blue
    0xFF7E2553,  // 2: Dark purple
    0xFF008751,  // 3: Dark green
    0xFFAB5236,  // 4: Brown
    0xFF5F574F,  // 5: Dark gray
    0xFFC2C3C7,  // 6: Light gray
    0xFFFFF1E8,  // 7: White
    0xFFFF004D,  // 8: Red
    0xFFFFA300,  // 9: Orange
    0xFFFFEC27,  // 10: Yellow
    0xFF00E436,  // 11: Green
    0xFF29ADFF,  // 12: Blue
    0xFF83769C,  // 13: Indigo
    0xFFFF77A8,  // 14: Pink
    0xFFFFCCAA,  // 15: Peach
};

// Extended 16 colors (TIC-80 style)
static constexpr uint32_t PALETTE_EXTENDED[16] = {
    0xFF1A1C2C,  // 16: Very dark blue
    0xFF5D275D,  // 17: Dark purple 2
    0xFFB13E53,  // 18: Red-purple
    0xFFEF7D57,  // 19: Light orange
    0xFFFFCD75,  // 20: Light yellow
    0xFFA7F070,  // 21: Light green
    0xFF38B764,  // 22: Medium green
    0xFF257179,  // 23: Teal
    0xFF29366F,  // 24: Navy
    0xFF3B5DC9,  // 25: Royal blue
    0xFF41A6F6,  // 26: Sky blue
    0xFF73EFF7,  // 27: Cyan
    0xFFF4F4F4,  // 28: Off-white
    0xFF94B0C2,  // 29: Light blue-gray
    0xFF566C86,  // 30: Blue-gray
    0xFF333C57,  // 31: Dark blue-gray
};
```

---

### **Cambios Necesarios en el Código**

#### **1. AestheticLayer.h/cpp**
```cpp
// Actualizar paleta de 16 a 32 colores
static constexpr int PALETTE_SIZE = 32;  // Was 16
static constexpr uint32_t PALETTE[32] = { /* ... */ };
```

#### **2. SpriteEditor.h**
```cpp
// Layout constants
static constexpr int PALETTE_COLS = 4;  // Mantener
static constexpr int PALETTE_ROWS = 8;  // Was 4, now 8
static constexpr int COLOR_BOX_SIZE = 12;  // Mantener
```

#### **3. SpriteEditor.cpp - RenderPalette()**
```cpp
void SpriteEditor::RenderPalette(AestheticLayer& renderer) {
    for (int row = 0; row < 8; row++) {  // Was 4, now 8
        for (int col = 0; col < 4; col++) {
            int colorIndex = row * 4 + col;  // 0-31
            // ... render logic
        }
    }
}
```

#### **4. Compatibilidad con Sprites Existentes**
- Los sprites existentes usan índices 0-15 (primeros 16 colores)
- **NO romper compatibilidad**: Los índices 0-15 deben mantenerse iguales
- Los nuevos colores 16-31 son opcionales

---

### **Testing Checklist**
- [ ] Paleta de 32 colores se renderiza correctamente
- [ ] Clicks en todos los 32 colores funcionan
- [ ] Sprites existentes (0-15) siguen viéndose igual
- [ ] Border blanco alrededor de la paleta se ajusta
- [ ] No hay overlap con otros elementos de UI

---

## 🏴 **FEATURE 2: Sprite Flags (PICO-8/TIC-80 Style)**

### **Concepto de Flags**

Los **sprite flags** son 8 bits booleanos por sprite que el usuario puede activar/desactivar para marcar propiedades del sprite (colisión, animación, tipo, etc.).

**Ejemplos de uso:**
```lua
-- PICO-8
if fget(sprite_id, 0) then  -- Check flag 0
    -- This sprite is solid
end

fset(sprite_id, 1, true)  -- Set flag 1
```

---

### **Diseño de UI para Flags**

**Opción A: Panel inferior (debajo de spritesheet)**
```
┌─────────────────────────────────┐
│  Spritesheet Grid (16x8)        │
├─────────────────────────────────┤
│ FLAGS: [0][1][2][3][4][5][6][7] │  ← 8 checkboxes
│ Sprite #12  "Solid Platform"     │  ← Label opcional
└─────────────────────────────────┘
```

**Opción B: Panel derecho (junto a paleta)**
```
┌──────────┐
│  Palette │
│  32x1    │
├──────────┤
│  FLAGS   │
│ [0] [1]  │
│ [2] [3]  │
│ [4] [5]  │
│ [6] [7]  │
└──────────┘
```

**Opción C: Editor modal (popup al hacer doble-click en sprite)**
```
┌─────────────────────────┐
│ Edit Sprite #12 Flags   │
├─────────────────────────┤
│ [ ] Flag 0: Solid       │
│ [x] Flag 1: Platform    │
│ [ ] Flag 2: Hazard      │
│ [ ] Flag 3: Collectible │
│ [ ] Flag 4: Animated    │
│ [ ] Flag 5: Background  │
│ [ ] Flag 6: Reserved    │
│ [ ] Flag 7: Reserved    │
├─────────────────────────┤
│        [OK] [Cancel]    │
└─────────────────────────┘
```

---

### **Estructura de Datos**

```cpp
// En SpriteEditor.h
class SpriteEditor {
private:
    // Current sprite data
    uint8_t canvas[8][8];           // Pixel data
    uint8_t spriteFlags[256];       // 8 bits per sprite (256 sprites)
    
    // En el archivo .spr (PNG metadata o archivo separado)
    // spritesheet.png (imagen 128x128)
    // spritesheet.flags (archivo binario 256 bytes)
};

// Funciones para flags
void SetSpriteFlag(int spriteIndex, int flagIndex, bool value);
bool GetSpriteFlag(int spriteIndex, int flagIndex);
void ToggleSpriteFlag(int spriteIndex, int flagIndex);
```

---

### **Formato de Archivo**

**Opción A: Archivo separado `.flags`**
```
spritesheet.png     (128x128 image, 256 sprites)
spritesheet.flags   (256 bytes, 1 byte per sprite)
```

**Pros:** Simple, fácil de implementar  
**Contras:** 2 archivos por spritesheet

**Opción B: PNG Metadata (tEXt chunk)**
```
PNG file with custom chunk:
  - IHDR (image header)
  - PLTE (palette)
  - tEXt "SpriteFlags" = "00 01 03 FF ..." (hex string)
  - IDAT (image data)
  - IEND
```

**Pros:** 1 solo archivo  
**Contras:** Más complejo de implementar

**Opción C: JSON sidecar**
```json
{
  "spritesheet": "spritesheet.png",
  "sprites": [
    { "id": 0, "flags": [false, false, false, false, false, false, false, false] },
    { "id": 1, "flags": [true, true, false, false, false, false, false, false] },
    ...
  ]
}
```

**Pros:** Legible, extensible  
**Contras:** Verbose, archivo extra

---

### **Implementación Recomendada**

1. **Usar archivo separado `.flags`** (256 bytes)
2. **UI: Panel inferior debajo de spritesheet** (Opción A)
3. **Auto-guardar flags** al guardar spritesheet
4. **API Lua** para acceder a flags desde cartridges

---

### **API Lua Propuesta**

```lua
-- Get sprite flag
local is_solid = fget(sprite_id, flag_index)

-- Set sprite flag
fset(sprite_id, flag_index, true)  -- Set to true
fset(sprite_id, flag_index, false) -- Set to false

-- Get all 8 flags as bitmask (0-255)
local flags = fget(sprite_id)

-- Set all 8 flags from bitmask
fset(sprite_id, 0b10101010)  -- Binary literal
```

---

### **Cambios Necesarios**

#### **1. SpriteEditor.h**
```cpp
// Add flags data
uint8_t spriteFlags[256];  // 8 bits per sprite

// Add flag UI
void RenderFlagPanel(AestheticLayer& renderer);
void HandleFlagClick(int mouseX, int mouseY);

// Flag API
void SetSpriteFlag(int spriteIndex, int flagBit, bool value);
bool GetSpriteFlag(int spriteIndex, int flagBit);
uint8_t GetSpriteFlagsAll(int spriteIndex);
void SetSpriteFlagsAll(int spriteIndex, uint8_t flags);
```

#### **2. File I/O**
```cpp
// Save flags to .flags file
void SaveSpriteFlags(const std::string& flagsPath);

// Load flags from .flags file
void LoadSpriteFlags(const std::string& flagsPath);
```

#### **3. Lua Bindings** (future)
```cpp
// In LuaAPI.cpp
int lua_fget(lua_State* L);  // fget(sprite, flag?)
int lua_fset(lua_State* L);  // fset(sprite, flag, value?)
```

---

## 📋 **CHECKLIST DE IMPLEMENTACIÓN**

### **Fase 1: Paleta de 32 Colores** (Prioridad Alta)
- [ ] Actualizar `PALETTE_SIZE` a 32
- [ ] Definir los 16 colores extendidos
- [ ] Cambiar `PALETTE_ROWS` de 4 a 8
- [ ] Actualizar `RenderPalette()` para 32 colores
- [ ] Actualizar `HandlePaletteClick()` para 32 colores
- [ ] Testing: Verificar que sprites viejos funcionan
- [ ] Testing: Verificar que nuevos colores funcionan

### **Fase 2: Sprite Flags** (Prioridad Media)
- [ ] Agregar `uint8_t spriteFlags[256]` a SpriteEditor
- [ ] Implementar `SetSpriteFlag()` / `GetSpriteFlag()`
- [ ] Diseñar panel de UI para flags
- [ ] Implementar `RenderFlagPanel()`
- [ ] Implementar `HandleFlagClick()`
- [ ] Implementar `SaveSpriteFlags()` / `LoadSpriteFlags()`
- [ ] Testing: Guardar/cargar flags correctamente
- [ ] Documentar formato de archivo `.flags`

### **Fase 3: Lua API** (Futuro)
- [ ] Implementar `lua_fget()`
- [ ] Implementar `lua_fset()`
- [ ] Agregar a registry de Lua
- [ ] Escribir ejemplos de uso
- [ ] Testing con cartridges de prueba

---

## 🎯 **DECISIONES PENDIENTES**

1. **Paleta: ¿8x4 u 4x8?** → Recomendación: **4x8** (vertical)
2. **Flags UI: ¿Panel inferior o derecho?** → Recomendación: **Panel inferior**
3. **Flags storage: ¿Archivo separado o PNG metadata?** → Recomendación: **Archivo separado** (.flags)
4. **¿Implementar labels para flags?** → Opcional, puede esperar

---

## 🔗 **Referencias**

**PICO-8 Sprite Flags:**
- https://pico-8.fandom.com/wiki/Fget
- https://pico-8.fandom.com/wiki/Fset

**TIC-80 Palette:**
- https://github.com/nesbox/TIC-80/wiki/palette
- 32 colors, 4x8 grid

**PNG Metadata:**
- http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html

---

**Última actualización:** 2025-12-20 05:05 AM  
**Próxima revisión:** Mañana, decidir qué implementar primero
