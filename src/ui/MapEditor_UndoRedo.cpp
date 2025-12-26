// MapEditor Undo/Redo System
// Tracks tile changes across all layers for undo/redo functionality

#include "ui/MapEditor.h"
#include <iostream>

// ===== Undo/Redo System =====

void MapEditor::SaveUndoState(const std::string& actionName) {
    // Remove any redo history (everything after current index)
    if (undoIndex >= 0 && undoIndex < (int)undoStack.size() - 1) {
        undoStack.erase(undoStack.begin() + undoIndex + 1, undoStack.end());
    }
    
    // Create new undo state
    UndoState state;
    state.actionName = actionName;
    state.layer = activeLayer;
    
    // Save entire layer data (8192 tiles)
    state.layerData = layers[activeLayer].data;
    
    undoStack.push_back(state);
    
    // Limit stack size to MAX_UNDO_STACK
    if (undoStack.size() > MAX_UNDO_STACK) {
        undoStack.erase(undoStack.begin());
    } else {
        undoIndex++;
    }
    
    std::cout << "[MapEditor] Undo state saved: " << actionName 
              << " (stack size: " << undoStack.size() << ")" << std::endl;
}

void MapEditor::Undo() {
    if (undoIndex <= 0) {
        ShowToast("Nothing to undo");
        return;
    }
    
    undoIndex--;
    const UndoState& state = undoStack[undoIndex];
    
    // Restore layer data
    layers[state.layer].data = state.layerData;
    activeLayer = state.layer;
    
    ShowToast("Undo: " + state.actionName);
    std::cout << "[MapEditor] Undo: " << state.actionName << std::endl;
}

void MapEditor::Redo() {
    if (undoIndex >= (int)undoStack.size() - 1) {
        ShowToast("Nothing to redo");
        return;
    }
    
    undoIndex++;
    const UndoState& state = undoStack[undoIndex];
    
    // Restore layer data
    layers[state.layer].data = state.layerData;
    activeLayer = state.layer;
    
    ShowToast("Redo: " + state.actionName);
    std::cout << "[MapEditor] Redo: " << state.actionName << std::endl;
}

void MapEditor::ClearUndoHistory() {
    undoStack.clear();
    undoIndex = -1;
    std::cout << "[MapEditor] Undo history cleared" << std::endl;
}
