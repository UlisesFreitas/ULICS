
// ===== Undo/Redo System =====

void SpriteEditor::SaveHistoryState() {
    // Remove any redo history (everything after current index)
    if (historyIndex >= 0 && historyIndex < (int)undoHistory.size() - 1) {
        undoHistory.erase(undoHistory.begin() + historyIndex + 1, undoHistory.end());
    }
    
    // Save current canvas state
    uint8_t stateCopy[8][8];
    std::memcpy(stateCopy, canvas, sizeof(canvas));
    undoHistory.push_back(stateCopy);
    
    // Limit history size
    if (undoHistory.size() > MAX_HISTORY) {
        undoHistory.erase(undoHistory.begin());
    } else {
        historyIndex++;
    }
}

void SpriteEditor::Undo() {
    if (historyIndex <= 0) return;  // No history to undo
    
    historyIndex--;
    std::memcpy(canvas, undoHistory[historyIndex], sizeof(canvas));
    
    // Update spritesheet
    std::memcpy(spriteSheet[currentSpriteIndex], canvas, sizeof(canvas));
}

void SpriteEditor::Redo() {
    if (historyIndex >= (int)undoHistory.size() - 1) return;  // No history to redo
    
    historyIndex++;
    std::memcpy(canvas, undoHistory[historyIndex], sizeof(canvas));
    
    // Update spritesheet
    std::memcpy(spriteSheet[currentSpriteIndex], canvas, sizeof(canvas));
}
