#include "editor/text/UndoRedoManager.h"

namespace UliCS {

UndoRedoManager::UndoRedoManager(int maxSize)
    : maxStackSize(maxSize) {
}

UndoRedoManager::~UndoRedoManager() {
}

void UndoRedoManager::PushUndo(const EditAction& action) {
    undoStack.push_back(action);
    
    // Limit stack size - remove oldest if exceeds max
    if (static_cast<int>(undoStack.size()) > maxStackSize) {
        undoStack.erase(undoStack.begin());
    }
    
    // Clear redo stack when new action is performed
    ClearRedoStack();
}

const UndoRedoManager::EditAction* UndoRedoManager::PeekUndo() const {
    if (undoStack.empty()) {
        return nullptr;
    }
    return &undoStack.back();
}

bool UndoRedoManager::PopUndo(EditAction& outAction) {
    if (undoStack.empty()) {
        return false;
    }
    
    outAction = undoStack.back();
    undoStack.pop_back();
    
    // Move to redo stack
    redoStack.push_back(outAction);
    
    return true;
}

bool UndoRedoManager::PopRedo(EditAction& outAction) {
    if (redoStack.empty()) {
        return false;
    }
    
    outAction = redoStack.back();
    redoStack.pop_back();
    
    // Move back to undo stack
    undoStack.push_back(outAction);
    
    return true;
}

void UndoRedoManager::ClearRedoStack() {
    redoStack.clear();
}

void UndoRedoManager::ClearAll() {
    undoStack.clear();
    redoStack.clear();
}

bool UndoRedoManager::CanUndo() const {
    return !undoStack.empty();
}

bool UndoRedoManager::CanRedo() const {
    return !redoStack.empty();
}

int UndoRedoManager::GetUndoCount() const {
    return static_cast<int>(undoStack.size());
}

int UndoRedoManager::GetRedoCount() const {
    return static_cast<int>(redoStack.size());
}

} // namespace UliCS
