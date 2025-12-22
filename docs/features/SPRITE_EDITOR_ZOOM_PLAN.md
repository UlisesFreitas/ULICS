# SPRITE EDITOR - ZOOM FEATURE PLAN

**Fecha:** 2025-12-20 04:38 AM  
**Objetivo:** Implementar Zoom In/Out con botones en barra vertical

---

## 🎯 DISEÑO DE ZOOM

### **Niveles de Zoom Disponibles:**
```cpp
static constexpr int ZOOM_LEVELS[] = { 8, 12, 16, 20, 24 };
static constexpr int DEFAULT_ZOOM_INDEX = 2;  // 16x (actual)
```

**Zoom actual:** 16x (8x8 sprite = 128x128 display)  
**Zoom mínimo:** 8x (8x8 sprite = 64x64 display)  
**Zoom máximo:** 24x (8x8 sprite = 192x192 display)

---

## 🖼️ LAYOUT ACTUALIZADO

### **Barra Vertical (8 botones):**
1. **Grid** (toggle)
2. **Flip H**
3. **Flip V**
4. **Clear**
5. **Copy**
6. **Paste**
7. **Zoom In** (+)  ← NUEVO
8. **Zoom Out** (-)  ← NUEVO

---

## 🔧 CAMBIOS TÉCNICOS

### **1. SpriteEditor.h - Agregar variables de zoom**

```cpp
// Zoom variables
int currentZoomIndex = 2;  // Default 16x
static constexpr int ZOOM_LEVELS[] = { 8, 12, 16, 20, 24 };
static constexpr int NUM_ZOOM_LEVELS = 5;
```

### **2. Funciones de zoom**

```cpp
void ZoomIn() {
    if (currentZoomIndex < NUM_ZOOM_LEVELS - 1) {
        currentZoomIndex++;
        zoom = ZOOM_LEVELS[currentZoomIndex];
        Log("[Zoom] In: " + std::to_string(zoom) + "x");
    }
}

void ZoomOut() {
    if (currentZoomIndex > 0) {
        currentZoomIndex--;
        zoom = ZOOM_LEVELS[currentZoomIndex];
        Log("[Zoom] Out: " + std::to_string(zoom) + "x");
    }
}
```

### **3. Hotkeys para zoom**

```cpp
// Mouse wheel zoom
if (input.getMouseWheelDelta() != 0) {
    if (input.getMouseWheelDelta() > 0) {
        ZoomIn();
    } else {
        ZoomOut();
    }
}

// Keyboard shortcuts
if (input.isKeyPressed(SDL_SCANCODE_EQUALS)) ZoomIn();   // + key
if (input.isKeyPressed(SDL_SCANCODE_MINUS)) ZoomOut();   // - key
```

---

## 📐 AJUSTES DE LAYOUT

### **Constantes a actualizar:**

```cpp
// Toolbar Y position (subir 2px para separación)
static constexpr int TOOLBAR_Y = CANVAS_Y + CANVAS_SIZE + 6;  // Was 8, now 6 for 2px gap

// Utility bar con 8 botones (era 6)
// 8 botones * 16px = 128px total (mismo alto que canvas)
```

---

## 🎨 ICONOS PARA ZOOM

### **ID 23: Zoom In (+)**
```
   ██
   ██
██████
██████
   ██
   ██
```
Plus sign (+)

### **ID 24: Zoom Out (-)**
```


██████
██████


```
Minus sign (-)

---

## 🔄 LÓGICA DE RENDERIZADO

**Problema:** Si el zoom cambia, el canvas cambia de tamaño.

**Solución:** Recalcular `CANVAS_SIZE` dinámicamente:
```cpp
int GetCanvasSize() const {
    return 8 * zoom;  // Always 8x8 sprite
}
```

**Ajuste necesario:**
- Toolbar Y debe ajustarse dinámicamente: `CANVAS_Y + GetCanvasSize() + 6`
- O mantener el layout fijo y centrar el canvas según el zoom

---

## ⚙️ IMPLEMENTACIÓN RECOMENDADA

### **Opción A: Zoom con layout fijo (RECOMENDADO)**
- Canvas máximo: 192x192 (24x zoom)
- Reservar espacio para el zoom máximo
- Centrar el canvas si el zoom es menor

**Ventaja:** Layout estable, no se mueve la UI

### **Opción B: Zoom con layout dinámico**
- Canvas crece/decrece según zoom
- Toolbar se mueve arriba/abajo
- Utilitybar crece/decrece

**Ventaja:** Usa espacio eficientemente  
**Desventaja:** UI se mueve, puede ser confuso

---

## 📋 CHECKLIST DE IMPLEMENTACIÓN

### Fase 1: Ajustes de Layout ✅
- [x] Subir toolbar 2px
- [ ] Agregar borde blanco a toolbar
- [ ] Agregar borde blanco a utility bar
- [ ] Expandir utility bar a 8 botones

### Fase 2: Iconos de Zoom
- [ ] Crear icono Zoom In (ID 23)
- [ ] Crear icono Zoom Out (ID 24)
- [ ] Compilar SystemSprites

### Fase 3: Lógica de Zoom
- [ ] Agregar variables de zoom
- [ ] Implementar ZoomIn() / ZoomOut()
- [ ] Agregar clicks en botones
- [ ] Agregar hotkeys (mouse wheel, +/-)

### Fase 4: Testing
- [ ] Verificar zoom funciona
- [ ] Verificar layout se mantiene estable
- [ ] Verificar grid se adapta al zoom
- [ ] Verificar preview de líneas/rect funciona

---

## ⚠️ CONSIDERACIONES

**Grid:** Debe escalar con el zoom (cada píxel del sprite = zoom píxeles en pantalla)

**Preview:** Line/Rect preview debe funcionar con cualquier zoom

**Performance:** Rendering es O(64) (8x8), no debería afectar FPS

**Límites:** Zoom máximo 24x para no exceder límites de pantalla

---

**¿Proceder con la implementación?**
