// MapEditor Undo/Redo System
// Simple undo/redo matching SpriteEditor logic

#include "ui/MapEditor.h"
#include <iostream>
#include <fstream>
#include <ctime>

// Helper to log to file for debugging
static void LogToFile(const std::string& message) {
    std::ofstream logFile("map_editor_log.txt", std::ios::app);
    if (logFile.is_open()) {
        // Get timestamp
        time_t now = time(0);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localtime(&now));
        
        logFile << "[" << timestamp << "] " << message << std::endl;
        logFile.close();
    }
}

// ===== Undo/Redo System =====

void MapEditor::SaveUndoState(const std::string& actionName) {
    LogToFile(">>> SaveUndoState called: " + actionName);
    LogToFile("    Before: undoIndex=" + std::to_string(undoIndex) + ", stack size=" + std::to_string(undoStack.size()));
    
    // Remove any redo history (everything after current index)
    if (undoIndex >= 0 && undoIndex < (int)undoStack.size() - 1) {
        int removed = undoStack.size() - (undoIndex + 1);
        undoStack.erase(undoStack.begin() + undoIndex + 1, undoStack.end());
        LogToFile("    Removed " + std::to_string(removed) + " redo states");
    }
    
    // Save current layer state (AFTER modification)
    UndoState state;
    state.actionName = actionName;
    state.layer = activeLayer;
    state.layerData = layers[activeLayer].data;
    
    undoStack.push_back(state);
    
    // Limit stack size
    if (undoStack.size() > MAX_UNDO_STACK) {
        undoStack.erase(undoStack.begin());
        LogToFile("    Stack limit reached, removed oldest");
    } else {
        undoIndex++;
    }
    
    LogToFile("    After: undoIndex=" + std::to_string(undoIndex) + ", stack size=" + std::to_string(undoStack.size()));
}

void MapEditor::Undo() {
    LogToFile(">>> Undo called");
    LogToFile("    undoIndex=" + std::to_string(undoIndex) + ", stack size=" + std::to_string(undoStack.size()));
    
    if (undoIndex <= 0) {
        ShowToast("Nothing to undo");
        LogToFile("    Cannot undo (index <= 0)");
        return;
    }
    
    undoIndex--;
    const UndoState& state = undoStack[undoIndex];
    
    LogToFile("    Restoring: " + state.actionName + " at index " + std::to_string(undoIndex));
    
    // Restore layer data
    layers[state.layer].data = state.layerData;
    activeLayer = state.layer;
    
    ShowToast("Undo");
}

void MapEditor::Redo() {
    LogToFile(">>> Redo called");
    LogToFile("    undoIndex=" + std::to_string(undoIndex) + ", stack size=" + std::to_string(undoStack.size()));
    
    if (undoIndex >= (int)undoStack.size() - 1) {
        ShowToast("Nothing to redo");
        LogToFile("    Cannot redo (index >= size-1)");
        return;
    }
    
    undoIndex++;
    const UndoState& state = undoStack[undoIndex];
    
    LogToFile("    Restoring: " + state.actionName + " at index " + std::to_string(undoIndex));
    
    // Restore layer data
    layers[state.layer].data = state.layerData;
    activeLayer = state.layer;
    
    ShowToast("Redo");
}

void MapEditor::ClearUndoHistory() {
    LogToFile(">>> ClearUndoHistory called");
    undoStack.clear();
    undoIndex = -1;
    LogToFile("    Stack cleared, undoIndex=-1");
}
