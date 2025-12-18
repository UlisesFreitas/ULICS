# PLAN: Undo/Redo System - ENFOQUE SIMPLE

## PROBLEMA IDENTIFICADO
El sistema actual guarda estados DESPUÉS de modificar, pero la lógica está confusa.
Cuando pintas el MISMO pixel varias veces, todos los estados son idénticos.

## SOLUCIÓN: Sistema de Pila Simple

### Estructura de datos:
```cpp
vector<CanvasState> undoStack;  // Estados ANTES de cada modificación
vector<CanvasState> redoStack;  // Estados que se deshicieron
```

### Flujo de trabajo:

**1. MODIFICACIÓN (Pintar, Fill, etc.):**
```
1. Guardar estado ACTUAL en undoStack
2. Aplicar modificación al canvas
3. Limpiar redoStack (ya no hay redo disponible)
```

**2. UNDO (Ctrl+Z):**
```
1. Si undoStack está vacío → No hacer nada
2. Guardar estado ACTUAL en redoStack
3. Restaurar último estado de undoStack
4. Eliminar ese estado de undoStack
```

**3. REDO (Ctrl+Y):**
```
1. Si redoStack está vacío → No hacer nada
2. Guardar estado ACTUAL en undoStack
3. Restaurar último estado de redoStack
4. Eliminar ese estado de redoStack
```

### Ejemplo práctico:

```
Sprite vacío: []

Pinta pixel A:
  undoStack: [[]]        ← Estado sin nada
  canvas: [A]

Pinta pixel B:
  undoStack: [[], [A]]   ← Estado con A
  canvas: [A, B]

Pinta pixel C:
  undoStack: [[], [A], [A,B]]  ← Estado con A,B
  canvas: [A, B, C]

UNDO:
  redoStack: [[A,B,C]]   ← Guardamos estado actual
  undoStack: [[], [A]]   ← Quitamos último
  canvas: [A, B]         ← Restauramos

UNDO:
  redoStack: [[A,B,C], [A,B]]
  undoStack: [[]]
  canvas: [A]

UNDO:
  redoStack: [[A,B,C], [A,B], [A]]
  undoStack: []
  canvas: []             ← VACÍO

REDO:
  undoStack: [[]]        ← Guardamos estado actual
  redoStack: [[A,B,C], [A,B]]  ← Quitamos último
  canvas: [A]            ← Restauramos
```

## VENTAJAS:
- ✅ Simple y robusto
- ✅ Lógica clara e intuitiva
- ✅ Similar a cualquier editor (Photoshop, Word, etc.)
- ✅ Fácil de debugear

## IMPLEMENTACIÓN:
- Reemplazar sistema actual completamente
- Usar 2 stacks separados (undo/redo)
- Limpiar cuando cambias de sprite
