# SPRITE EDITOR - FUTURE ENHANCEMENTS

## 🎯 IMPORT/EXPORT SPRITESHEET UI

**Priority:** HIGH  
**Estimated Time:** 2-3 hours  
**Status:** Pending

---

### 📥 IMPORT SPRITESHEET

**Feature:** Permitir al usuario importar su propio spritesheet desde un archivo PNG externo.

#### UI:
- **Icono:** SystemSprites ID 6 (LOAD - 📂 Folder)
- **Ubicación:** Toolbar del Sprite Editor, junto a los tools
- **Hotkey sugerido:** Ctrl+O (Open)

#### Implementación del botón:
```cpp
// En RenderToolbar()
int x = CANVAS_X + (5 * (BUTTON_SIZE + BUTTON_SPACING)); // Después de los 5 tools

// Import button
uint8_t bgColor = UISystem::COLOR_DARK_GRAY;
renderer.RectFill(x, TOOLBAR_Y, BUTTON_SIZE, BUTTON_SIZE, bgColor);
systemSprites->DrawSprite(renderer, 
    static_cast<int>(SystemSprites::Icon::LOAD),  // ID 6
    x + 4, TOOLBAR_Y + 4, 1);
```

#### Comportamiento:
1. Usuario hace click en icono "Import"
2. Se abre file picker (diálogo de archivos)
3. Usuario selecciona un PNG
4. Sistema valida el archivo:
   - ✅ Debe ser 128×128 píxeles
   - ✅ Debe ser formato PNG válido
   - ✅ Preferiblemente grayscale o RGBA
5. Si es válido:
   - Reemplaza `spritesheet.png` en el cartridge actual
   - Actualiza el array `spriteSheet[256][8][8]` en memoria
   - Refresca la UI
   - Muestra mensaje: "Spritesheet imported successfully"
6. Si es inválido:
   - Muestra error: "Invalid PNG. Must be 128×128 pixels"

#### Validación:
```cpp
bool ValidateSpritesheetPNG(const char* path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    
    if (!data) return false;
    if (width != 128 || height != 128) {
        stbi_image_free(data);
        return false;
    }
    
    stbi_image_free(data);
    return true;
}
```

---

### 📤 EXPORT SPRITESHEET

**Feature:** Permitir al usuario exportar el spritesheet actual a cualquier ubicación.

#### UI:
- **Icono:** SystemSprites ID 5 (SAVE - 💾 Diskette)
- **Ubicación:** Toolbar del Sprite Editor, al lado del Import
- **Hotkey sugerido:** Ctrl+E (Export) o Ctrl+Shift+S

#### Implementación del botón:
```cpp
// En RenderToolbar()
int x = CANVAS_X + (6 * (BUTTON_SIZE + BUTTON_SPACING)); // Después del Import

// Export button
uint8_t bgColor = UISystem::COLOR_DARK_GRAY;
renderer.RectFill(x, TOOLBAR_Y, BUTTON_SIZE, BUTTON_SIZE, bgColor);
systemSprites->DrawSprite(renderer, 
    static_cast<int>(SystemSprites::Icon::SAVE),  // ID 5
    x + 4, TOOLBAR_Y + 4, 1);
```

#### Comportamiento:
1. Usuario hace click en icono "Export"
2. Se abre save dialog
3. Usuario elige:
   - **Ubicación:** Cualquier carpeta
   - **Nombre:** Por defecto `spritesheet_2025-12-18.png` o custom
4. Sistema guarda el PNG completo (128×128)
5. Muestra mensaje: "Exported to: C:/Users/.../spritesheet.png"

#### Naming Suggestions:
- `spritesheet_YYYY-MM-DD_HHMM.png` (con timestamp)
- `my_custom_sprites.png` (nombre del usuario)
- `{cartridge_name}_sprites.png` (basado en nombre del cart)

---

### 🛠️ IMPLEMENTACIÓN TÉCNICA

#### Opción 1: Native Windows API
```cpp
#include <windows.h>

std::string OpenFileDialog() {
    OPENFILENAME ofn;
    char szFile[260] = {0};
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "PNG Files\0*.png\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn)) {
        return std::string(szFile);
    }
    return "";
}
```

#### Opción 2: Cross-Platform Library
**nativefiledialog** - Recomendado para multi-plataforma
```cpp
#include <nfd.h>

std::string OpenFileDialog() {
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog("png", NULL, &outPath);
    
    if (result == NFD_OKAY) {
        std::string path(outPath);
        free(outPath);
        return path;
    }
    return "";
}
```

---

### 📋 UI MOCKUP

```
┌────────────────────────────────────────┐
│ Canvas (128x128) - 8x8 sprite          │
│                                        │
└────────────────────────────────────────┘
┌────────────────────────────────────────┐
│ [🖊️] [🪣] [📏] [▭] [💧] │ [📂] [💾] │
│  P    F    L    R    C     Import Export│
│                                        │
│ Pencil Fill Line Rect Picker           │
└────────────────────────────────────────┘
  ↑                        ↑
  5 tool buttons        2 new buttons
  (SystemSprites 0-4)   (SystemSprites 6,5)
```

**Layout:**
- Tools 0-4: Posiciones 0-4
- Import (ID 6): Posición 5  
- Export (ID 5): Posición 6
- Espaciado: 4px entre botones
- Tamaño: 16x16 cada botón

---

### ⚙️ MEJORAS FUTURAS DE NAMING

1. **Múltiples Spritesheets por Cartridge:**
   ```
   /cartridge
     /sprites
       - characters.png
       - tileset.png
       - ui_icons.png
   ```

2. **Prefijos Automáticos:**
   - `tileset_` para tiles del mapa
   - `chars_` para personajes
   - `ui_` para elementos de interfaz

3. **Sistema de Capas:**
   - Permitir múltiples capas de sprites
   - Exportar/Importar por capa

---

### 📊 CASOS DE USO

**Artista usando Aseprite:**
1. Crea sprites en Aseprite
2. Exporta como 128×128 PNG
3. Abre ULICS Sprite Editor
4. Click en Import 📂
5. Selecciona su PNG
6. ¡Listo! Sprites cargados

**Exportar para compartir:**
1. Termina de editar sprites
2. Click en Export 💾
3. Guarda como `my_awesome_sprites.png`
4. Comparte con otros desarrolladores

---

### ✅ TESTING CHECKLIST

- [ ] Import válido (128×128 PNG)
- [ ] Import inválido (tamaño incorrecto)
- [ ] Import inválido (no es PNG)
- [ ] Export a ubicación custom
- [ ] Export con nombre custom
- [ ] Cancelar import dialog
- [ ] Cancelar export dialog
- [ ] Verificar que sprites se actualizan en UI
- [ ] Verificar que Ctrl+S sigue funcionando
- [ ] Hotkeys Ctrl+O y Ctrl+E

---

## 🎨 BENEFICIOS

1. ✅ **Flexibilidad:** Artistas pueden usar sus herramientas favoritas
2. ✅ **Colaboración:** Fácil compartir sprites entre proyectos
3. ✅ **Backup:** Exportar para respaldo
4. ✅ **Workflow mejorado:** No depender solo del editor interno
5. ✅ **Profesional:** Feature estándar en todos los editores modernos

---

**Próximos pasos:**
1. Decidir: ¿API nativa de Windows o librería cross-platform?
2. Implementar file dialogs
3. Agregar íconos Import/Export al toolbar
4. Implementar validación
5. Testing exhaustivo
