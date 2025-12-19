# SPRITE EDITOR - PLAN: AGREGAR BARRAS UI

**Status:** 🟢 LISTO PARA IMPLEMENTAR  
**Prioridad:** ALTA  
**Tiempo estimado:** 30 minutos  
**Archivos:** 2 (SpriteEditor.h, SpriteEditor.cpp)

---

## 🎯 OBJETIVO

Agregar barras superior e inferior al Sprite Editor, idénticas al CodeEditor:
- **Barra superior:** Título "SPRITES" + número de sprite actual
- **Barra inferior:** Tool activa + color + info de undo

**Beneficios:**
- ✅ Consistencia visual con CodeEditor
- ✅ Espacio para futuro menú con iconos
- ✅ Feedback visual mejorado para el usuario

---

## 📋 CAMBIOS REQUERIDOS

### **1. Actualizar constantes en `SpriteEditor.h`**

**Buscar líneas alrededor de:**
```cpp
static constexpr int CANVAS_X = 16;
static constexpr int CANVAS_Y = 20;
```

**Cambiar a:**
```cpp
// Screen layout - updated for title/status bars
static constexpr int TITLE_BAR_H = 10;
static constexpr int STATUS_BAR_H = 10;

static constexpr int CANVAS_X = 16;
static constexpr int CANVAS_Y = 30;          // Era 20, subió 10px por title bar

static constexpr int PALETTE_X = 152;
static constexpr int PALETTE_Y = 30;         // Era 20, subió 10px

static constexpr int SHEET_X = 8;
static constexpr int SHEET_Y = 170;          // Era 160, subió 10px

static constexpr int TOOLBAR_Y = 230;        // Era 235, bajó 5px para status bar
```

**Líneas aproximadas:** 40-60 en el header

---

### **2. Actualizar `RenderCanvas()` en `SpriteEditor.cpp`**

No requiere cambios (usa las constantes CANVAS_X/Y que ya actualizamos).

---

### **3. Modificar `Render()` en `SpriteEditor.cpp`**

**Ubicación:** Línea ~145

**Cambio:**

**ANTES:**
```cpp
void SpriteEditor::Render(AestheticLayer& renderer) {
    if (!isActive) return;
    
    // Clear screen
    renderer.Clear(UISystem::COLOR_DARK_BLUE);
    
    // Render all components (PICO-8 style layout)
    RenderCanvas(renderer);        
    RenderPalette(renderer);       
    RenderSpritesheet(renderer);   
    RenderToolbar(renderer);       
}
```

**DESPUÉS:**
```cpp
void SpriteEditor::Render(AestheticLayer& renderer) {
    if (!isActive) return;
    
    // === Constants ===
    const int SCREEN_W = 256;
    const int SCREEN_H = 256;
    const int THEME_BAR = UISystem::COLOR_LIGHT_GRAY;
    const int THEME_BAR_TEXT = UISystem::COLOR_BACKGROUND;
    
    // Clear screen
    renderer.Clear(UISystem::COLOR_DARK_BLUE);
    
    // === TITLE BAR ===
    renderer.RectFill(0, 0, SCREEN_W, TITLE_BAR_H, THEME_BAR);
    renderer.Print("SPRITES", 4, 1, THEME_BAR_TEXT);
    
    // Sprite number (right side)
    char spriteNum[16];
    sprintf(spriteNum, "#%03d", currentSpriteIndex);
    int numX = SCREEN_W - (strlen(spriteNum) * 8) - 4;
    renderer.Print(spriteNum, numX, 1, THEME_BAR_TEXT);
    
    // === MAIN CONTENT ===
    RenderCanvas(renderer);        
    RenderPalette(renderer);       
    RenderSpritesheet(renderer);   
    RenderToolbar(renderer);       
    
    // === STATUS BAR ===
    int statusY = SCREEN_H - STATUS_BAR_H;
    renderer.RectFill(0, statusY, SCREEN_W, STATUS_BAR_H, THEME_BAR);
    
    // Tool name + selected color
    const char* toolNames[] = { "PENCIL", "FILL", "LINE", "RECT", "PICKER" };
    char status[64];
    sprintf(status, "%s C:%d", toolNames[static_cast<int>(currentTool)], selectedColor);
    renderer.Print(status, 2, statusY + 1, THEME_BAR_TEXT);
    
    // Undo info (right side)
    if (!undoStack.empty()) {
        char undoInfo[16];
        sprintf(undoInfo, "U:%d", static_cast<int>(undoStack.size()));
        int undoX = SCREEN_W - (strlen(undoInfo) * 8) - 2;
        renderer.Print(undoInfo, undoX, statusY + 1, THEME_BAR_TEXT);
    }
}
```

---

## ✅ CHECKLIST DE IMPLEMENTACIÓN

### Paso 1: Header (SpriteEditor.h)
- [ ] Abrir `src/ui/SpriteEditor.h`
- [ ] Buscar las constantes de layout (alrededor línea 40-60)
- [ ] Agregar `TITLE_BAR_H` y `STATUS_BAR_H`
- [ ] Actualizar valores de `CANVAS_Y`, `PALETTE_Y`, `SHEET_Y`, `TOOLBAR_Y`
- [ ] Guardar

### Paso 2: Implementation (SpriteEditor.cpp)
- [ ] Abrir `src/ui/SpriteEditor.cpp`
- [ ] Buscar método `Render()` (línea ~145)
- [ ] Reemplazar contenido según código de arriba
- [ ] Verificar que `sprintf` esté disponible (ya se usa en otras partes)
- [ ] Guardar

### Paso 3: Build & Test
- [ ] Compilar proyecto
- [ ] Abrir Sprite Editor (F2)
- [ ] Verificar:
  - ✅ Barra superior visible con "SPRITES" y "#000"
  - ✅ Barra inferior con tool name y color
  - ✅ Canvas no se solapa con barras
  - ✅ Spritesheet grid visible completo
  - ✅ Toolbar no solapa con status bar

---

## 🔍 VALIDATION TESTS

Después de implementar, verificar:

1. **Visual:**
   - ✅ Barras superior e inferior de 10px cada una
   - ✅ Color gris claro (igual que CodeEditor)
   - ✅ Texto negro sobre gris
   - ✅ Número de sprite actualiza al cambiar sprite

2. **Funcional:**
   - ✅ Todas las herramientas siguen funcionando
   - ✅ Click en canvas sigue funcionando
   - ✅ Click en palette sigue funcionando
   - ✅ Toolbar sigue funcionando
   - ✅ Spritesheet grid sigue funcionando

3. **Consistency:**
   - ✅ Comparar visualmente con CodeEditor (F1)
   - ✅ Alturas de barras idénticas
   - ✅ Colores idénticos

---

## 📦 ARCHIVOS A MODIFICAR

| Archivo | Cambios | Líneas aprox |
|---------|---------|--------------|
| `src/ui/SpriteEditor.h` | Agregar constantes, actualizar Y positions | ~10 líneas |
| `src/ui/SpriteEditor.cpp` | Expandir Render() con barras | ~30 líneas |

**Total:** ~40 líneas modificadas/agregadas

---

## 🚀 NEXT STEPS (DESPUÉS DE ESTO)

Una vez implementado esto, considerar:

1. **Menú de iconos en barra superior** (futuro global UI)
   - Iconos para navegación entre editores
   - Consistente con plan de Global UI

2. **Drag & Drop Import** (del roadmap)
   - Alta prioridad en roadmap
   - UX muy intuitiva

3. **Custom naming templates** (del roadmap)
   - Medio esfuerzo, medio impacto
   - Profesionaliza workflow

---

**Ready to implement!** 🎨
