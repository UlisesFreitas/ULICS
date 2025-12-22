# PLAN DE FINALIZACIÓN - CodeEditor Refactor
## Objetivo: Reducir CodeEditor.cpp de 1,435 → 350-400 líneas

---

## TAREAS PENDIENTES (Fase 3 - CRÍTICA)

### ✅ COMPLETADO:
- [x] Fase 1: Extraer módulos (UndoRedoManager, TextSelection, TextBuffer, Scrollbar, SyntaxHighlighter)
- [x] Fase 2: Integrar módulos básicos
- [x] Migrar parcialmente TextBuffer

### ❌ FALTA (LO CRÍTICO):

#### 1. Extraer EditorRenderer (~300 líneas) ⚠️ PRIORITARIO
**Objetivo:** Mover TODO el rendering a módulo separado

**Archivos a crear:**
- `src/editor/rendering/EditorRenderer.h`
- `src/editor/rendering/EditorRenderer.cpp`

**Qué mover:**
- Todo el método `Render()` actual (~250 líneas)
- Rendering de líneas de texto
- Rendering de cursor
- Rendering de número de líneas
- Rendering de mensajes (saved/reloaded)
- Cálculos de layout (TEXT_X, EDITOR_TOP, etc.)

**Resultado:**
```cpp
// CodeEditor.cpp - Método Render() simplificado
void CodeEditor::Render(AestheticLayer& layer, UISystem& ui) {
    renderer.Render(layer, ui, cursorLine, cursorCol, scrollY, scrollX);
}
```

---

#### 2. Extraer EditorInput (~300 líneas) ⚠️ PRIORITARIO
**Objetivo:** Mover TODO el input handling a módulo separado

**Archivos a crear:**
- `src/editor/input/EditorInput.h`
- `src/editor/input/EditorInput.cpp`

**Qué mover:**
- Mouse handling (clicks, drag selection)
- Keyboard shortcuts (Ctrl+C/V/X/A/Z/Y)
- Text input handling
- Tab handling
- Arrow key handling con repeat
- Todos los métodos MoveCursor* (Left/Right/Up/Down/Home/End/Top/Bottom)
- PageUp/PageDown
- Métodos helper de cursor (EnsureCursorVisible, ClampCursor)

**Resultado:**
```cpp
// CodeEditor.cpp - Método Update() simplificado
void CodeEditor::Update(InputManager& input) {
    inputHandler.Update(input, cursorLine, cursorCol, scrollY, scrollX);
    
    // Solo la lógica específica de orchestrator
    if (fileExplorer) {
        fileExplorer->Update(input);
    }
    
    // File watching
    if (fileWatchingEnabled && !currentFile.empty()) {
        CheckFileForChanges();
    }
}
```

---

#### 3. Limpiar y Consolidar (~50 líneas reducidas)
**Objetivo:** Eliminar código duplicado restante

**Tareas:**
- Eliminar métodos MoveCursor* de CodeEditor (movidos a EditorInput)
- Eliminar helpers de rendering (movidos a EditorRenderer)
- Consolidar constantes de layout en EditorRenderer
- Actualizar CMakeLists.txt con nuevos archivos

---

## ORDEN DE EJECUCIÓN:

### Paso 1: Crear EditorRenderer
1. Crear archivos header/cpp
2. Mover código de Render()
3. Actualizar CMakeLists.txt
4. Compilar y probar

### Paso 2: Crear EditorInput  
1. Crear archivos header/cpp
2. Mover código de Update() y helpers
3. Actualizar CMakeLists.txt
4. Compilar y probar

### Paso 3: Verificación Final
1. Contar líneas finales
2. Verificar funcionalidad completa
3. Actualizar plan-ulics-2.json con "completed"
4. Marcar tarea 2.0.5.5 como DONE

---

## ESTIMACIÓN:
- EditorRenderer: 45 min
- EditorInput: 60 min
- Testing/Cleanup: 15 min
**Total: ~2 horas**

---

## RESULTADO ESPERADO:

```
CodeEditor.cpp: 
  ANTES: 1,435 líneas
  DESPUÉS: ~350-400 líneas (orchestrator)
  REDUCCIÓN: ~1,000 líneas (-70%)
```

**¡EMPEZAMOS AHORA!** 🚀
