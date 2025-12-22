# SPRITE EDITOR - PRÓXIMAS MEJORAS

**Fecha:** 2025-12-19  
**Estado:** ✅ Barras UI completadas, planificando siguiente fase

---

## ✅ COMPLETADO

- ✅ Barras superior e inferior (title bar + status bar)
- ✅ Canvas 8x8 con zoom 16x
- ✅ Herramientas: Pencil, Fill, Line, Rectangle, Color Picker
- ✅ Undo/Redo (50 niveles)
- ✅ Import/Export con file dialogs
- ✅ Recent files
- ✅ SystemSprites icons para tools

---

## 🎯 PRÓXIMAS MEJORAS (Propuestas)

### **1. Preview Visual de Line y Rectangle** ⭐ PRIORIDAD ALTA

**Problema actual:**
- Al usar Line o Rectangle con drag, no ves la preview mientras arrastras
- Solo se dibuja cuando sueltas el mouse
- Dificulta hacer líneas/rectángulos precisos

**Solución:**
Mostrar una **preview temporal** mientras arrastras (similar a cualquier programa de dibujo).

**Implementación:**

```cpp
// En SpriteEditor::Render(), después de RenderCanvas()
void SpriteEditor::RenderDragPreview(AestheticLayer& renderer) {
    if (!isDragging) return;
    
    if (currentTool == Tool::LINE) {
        // Dibujar línea de preview (usando el algoritmo de Bresenham)
        // pero en el renderer, NO en el canvas
        
        int startScreenX = CANVAS_X + (dragStartX * CANVAS_ZOOM);
        int startScreenY = CANVAS_Y + (dragStartY * CANVAS_ZOOM);
        
        int mouseX = input.getMouseX();
        int mouseY = input.getMouseY();
        int endCanvasX = ScreenToCanvasX(mouseX);
        int endCanvasY = ScreenToCanvasY(mouseY);
        
        if (IsInCanvas(endCanvasX, endCanvasY)) {
            int endScreenX = CANVAS_X + (endCanvasX * CANVAS_ZOOM);
            int endScreenY = CANVAS_Y + (endCanvasY * CANVAS_ZOOM);
            
            // Dibujar la línea de preview (semi-transparente o con color especial)
            renderer.Line(startScreenX + CANVAS_ZOOM/2, 
                         startScreenY + CANVAS_ZOOM/2,
                         endScreenX + CANVAS_ZOOM/2, 
                         endScreenY + CANVAS_ZOOM/2, 
                         UISystem::COLOR_YELLOW);  // Color de preview
        }
    }
    
    if (currentTool == Tool::RECT) {
        // Similar para rectángulo
        // Dibujar outline del rectángulo
    }
}
```

**Cambios necesarios:**
- Agregar método `RenderDragPreview()` a SpriteEditor
- Llamar después de `RenderCanvas()` en `Render()`
- Pasar `InputManager&` como parámetro (para obtener mouse position)

**Estimación:** 1 hora

---

### **2. Cursor/Puntero Personalizado del Mouse** ⏳ PRIORIDAD MEDIA

**Opciones:**

#### **Opción A: Highlight del pixel bajo el cursor**
No usa icono de cursor, sino resalta el píxel donde estás apuntando.

```cpp
void SpriteEditor::RenderCursorHighlight(AestheticLayer& renderer, InputManager& input) {
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    
    // Check if mouse is over canvas
    if (mouseX >= CANVAS_X && mouseX < CANVAS_X + CANVAS_SIZE &&
        mouseY >= CANVAS_Y && mouseY < CANVAS_Y + CANVAS_SIZE) {
        
        int canvasX = ScreenToCanvasX(mouseX);
        int canvasY = ScreenToCanvasY(mouseY);
        
        // Dibujar border del pixel bajo el cursor
        int screenX = CANVAS_X + (canvasX * CANVAS_ZOOM);
        int screenY = CANVAS_Y + (canvasY * CANVAS_ZOOM);
        
        renderer.Rect(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM, 
                     UISystem::COLOR_WHITE);  // Border blanco
    }
}
```

**Ventajas:**
- Fácil de implementar (15 min)
- No requiere nuevos iconos
- Ayuda a ver exactamente dónde clickearás

#### **Opción B: Icono de cursor custom**
Requiere agregar iconos al SystemSprites.

**Iconos necesarios:**
- ID 16: Cruz/Crosshair (para pencil/picker)
- ID 17: Mano (para drag/pan)  
- ID 18: Balde (para fill visible en cursor)

**Implementación:**
```cpp
void SpriteEditor::RenderCustomCursor(AestheticLayer& renderer, InputManager& input) {
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    
    // Esconder cursor de Windows (necesita SDL_ShowCursor(SDL_DISABLE))
    
    // Dibujar icono según tool activa
    int iconId = 0;
    switch (currentTool) {
        case Tool::PENCIL: iconId = 16; break;  // Crosshair
        case Tool::FILL:   iconId = 18; break;  // Bucket
        case Tool::PICKER: iconId = 16; break;  // Crosshair
        default: iconId = 16; break;
    }
    
    if (systemSprites) {
        systemSprites->DrawSprite(renderer, iconId, mouseX - 4, mouseY - 4, 1);
    }
}
```

**Ventajas:**
- Más visual y retro
- Indica claramente qué tool está activa

**Desventajas:**
- Requiere diseñar/agregar iconos a SystemSprites
- Puede causar lag si no se optimiza

**Estimación:** 2-3 horas (incluyendo diseño de iconos)

---

## 📊 COMPARATIVA

| Feature | Prioridad | Tiempo | Impacto | Complejidad |
|---------|-----------|--------|---------|-------------|
| **Line/Rect Preview** | ALTA | 1h | ALTO | MEDIA |
| **Cursor Highlight (Opción A)** | MEDIA | 15min | MEDIO | BAJA |
| **Cursor Custom Icons (Opción B)** | BAJA | 3h | MEDIO | ALTA |

---

## 🎨 ICONOS DISPONIBLES (SYSTEM_SPRITES.md)

**Ya implementados (0-15):**
```
0  - PENCIL    🖊️
1  - FILL      🪣
2  - LINE      📏
3  - RECT      ▭
4  - PICKER    💧
5  - SAVE      💾
6  - LOAD      📂
7  - PLAY      ▶️
8  - STOP      ⏹️
9  - ARROW_LEFT   ◀️
10 - ARROW_RIGHT  ▶️
11 - ARROW_UP     🔼
12 - ARROW_DOWN   🔽
13 - FOLDER    📁
14 - FILE      📄
15 - GEAR      ⚙️
```

**Slots disponibles (16-255):**
- 16-255: Libres para custom icons

**Iconos sugeridos para agregar:**
- ID 16: **CROSSHAIR** (cursor para pencil/picker)
- ID 17: **HAND** (cursor para pan/move)
- ID 18: **BUCKET_CURSOR** (version pequeña del bucket para cursor)
- ID 19: **UNDO** (icono de undo)
- ID 20: **REDO** (icono de redo)

---

## 🚀 RECOMENDACIÓN DE IMPLEMENTACIÓN

### **Fase 1: Quick Win (45 min)**
1. ✅ **Line/Rect Preview** (1h)
   - Alto impacto UX
   - Fácil de implementar
   - No requiere assets nuevos

2. ✅ **Cursor Highlight** (15min)
   - Super rápido
   - Ayuda mucho a precisión
   - No requiere iconos nuevos

### **Fase 2: Polish (Opcional, 3h)**
3. ⏳ **Custom Cursor Icons**
   - Diseñar iconos 16-20
   - Implementar renderizado de cursor
   - Esconder cursor de sistema

---

## 📝 CÓDIGO DE EJEMPLO: LINE PREVIEW

```cpp
// En SpriteEditor.h
void RenderDragPreview(AestheticLayer& renderer, InputManager& input);

// En SpriteEditor.cpp, dentro de Render()
void SpriteEditor::Render(AestheticLayer& renderer) {
    // ... existing code ...
    
    RenderCanvas(renderer);
    RenderPalette(renderer);
    RenderSpritesheet(renderer);
    RenderToolbar(renderer);
    
    // NUEVO: Render drag preview AFTER canvas (so it's on top)
    if (isDragging) {
        RenderDragPreview(renderer, input);  // ⚠️ Necesita InputManager&
    }
    
    // ... status bar ...
}

void SpriteEditor::RenderDragPreview(AestheticLayer& renderer, InputManager& input) {
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    
    int endCanvasX = ScreenToCanvasX(mouseX);
    int endCanvasY = ScreenToCanvasY(mouseY);
    
    if (!IsInCanvas(endCanvasX, endCanvasY)) return;
    
    if (currentTool == Tool::LINE) {
        // Bresenham para calcular todos los puntos
        std::vector<std::pair<int,int>> points = GetLinePoints(
            dragStartX, dragStartY, endCanvasX, endCanvasY);
        
        // Dibujar cada punto como un rectangulo en pantalla
        for (auto [x, y] : points) {
            int screenX = CANVAS_X + (x * CANVAS_ZOOM);
            int screenY = CANVAS_Y + (y * CANVAS_ZOOM);
            
            // Semi-transparente o color diferente
            renderer.Rect(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM, 
                         UISystem::COLOR_YELLOW);
        }
    }
    
    if (currentTool == Tool::RECT) {
        // Similar pero solo outline
        int minX = std::min(dragStartX, endCanvasX);
        int maxX = std::max(dragStartX, endCanvasX);
        int minY = std::min(dragStartY, endCanvasY);
        int maxY = std::max(dragStartY, endCanvasY);
        
        // Draw preview rectangle outline
        for (int x = minX; x <= maxX; x++) {
            // Top edge
            int screenX = CANVAS_X + (x * CANVAS_ZOOM);
            int screenY = CANVAS_Y + (minY * CANVAS_ZOOM);
            renderer.Rect(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM, UISystem::COLOR_YELLOW);
            
            // Bottom edge
            screenY = CANVAS_Y + (maxY * CANVAS_ZOOM);
            renderer.Rect(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM, UISystem::COLOR_YELLOW);
        }
        
        for (int y = minY + 1; y < maxY; y++) {
            // Left edge
            int screenX = CANVAS_X + (minX * CANVAS_ZOOM);
            int screenY = CANVAS_Y + (y * CANVAS_ZOOM);
            renderer.Rect(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM, UISystem::COLOR_YELLOW);
            
            // Right edge
            screenX = CANVAS_X + (maxX * CANVAS_ZOOM);
            renderer.Rect(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM, UISystem::COLOR_YELLOW);
        }
    }
}

// Helper function
std::vector<std::pair<int,int>> SpriteEditor::GetLinePoints(int x1, int y1, int x2, int y2) {
    std::vector<std::pair<int,int>> points;
    
    // Bresenham's algorithm
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int x = x1, y = y1;
    while (true) {
        points.push_back({x, y});
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx) { err += dx; y += sy; }
    }
    
    return points;
}
```

---

## ⚠️ NOTA IMPORTANTE

Para implementar `RenderDragPreview()` necesitamos **pasar InputManager&** al método `Render()`.

**Cambio en firma:**
```cpp
// ANTES:
void Render(AestheticLayer& renderer);

// DESPUÉS:
void Render(AestheticLayer& renderer, InputManager& input);
```

**Esto requiere actualizar:**
- `SpriteEditor.h` - Declaración
- `SpriteEditor.cpp` - Implementación
- `Engine.cpp` - Llamada a `spriteEditor->Render(renderer, input)`

---

## 🎯 PRÓXIMO PASO RECOMENDADO

**Implementar Line/Rect Preview** (1 hora)

**Beneficios:**
- ✅ Alto impacto en UX
- ✅ Profesionaliza el editor
- ✅ Feature estándar en todo software de dibujo

**¿Proceder?**
