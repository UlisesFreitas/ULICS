#pragma once

#include <string>
#include <vector>

namespace UliCS {

/**
 * @brief Manages undo/redo operations for text editing
 * 
 * Maintains a stack of edit actions with configurable maximum size.
 * Supports INSERT_CHAR, DELETE_CHAR, INSERT_LINE, DELETE_LINE, and REPLACE_TEXT operations.
 */
class UndoRedoManager {
public:
    /**
     * @brief Types of edit actions that can be undone/redone
     */
    enum ActionType {
        INSERT_CHAR,   // Character inserted
        DELETE_CHAR,   // Character deleted
        INSERT_LINE,   // New line created
        DELETE_LINE,   // Line removed
        REPLACE_TEXT   // Text replaced (e.g., selection deletion)
    };
    
    /**
     * @brief Represents a single edit action
     */
    struct EditAction {
        ActionType type;
        int line;           // Line number where action occurred
        int col;            // Column number where action occurred
        std::string oldText; // Text before the action (for undo)
        std::string newText; // Text after the action (for redo)
    };
    
    UndoRedoManager(int maxStackSize = 100);
    ~UndoRedoManager();
    
    /**
     * @brief Push a new action onto the undo stack
     * Clears the redo stack
     */
    void PushUndo(const EditAction& action);
    
    /**
     * @brief Get the last action from undo stack without removing it
     * @return Pointer to action, or nullptr if stack is empty
     */
    const EditAction* PeekUndo() const;
    
    /**
     * @brief Pop and return the last action from undo stack
     * Moves it to redo stack
     * @return true if an action was popped
     */
    bool PopUndo(EditAction& outAction);
    
    /**
     * @brief Pop and return the last action from redo stack
     * Moves it to undo stack
     * @return true if an action was popped
     */
    bool PopRedo(EditAction& outAction);
    
    /**
     * @brief Clear the redo stack
     */
    void ClearRedoStack();
    
    /**
     * @brief Clear both undo and redo stacks
     */
    void ClearAll();
    
    /**
     * @brief Check if undo stack has actions
     */
    bool CanUndo() const;
    
    /**
     * @brief Check if redo stack has actions
     */
    bool CanRedo() const;
    
    /**
     * @brief Get number of actions in undo stack
     */
    int GetUndoCount() const;
    
    /**
     * @brief Get number of actions in redo stack
     */
    int GetRedoCount() const;

private:
    std::vector<EditAction> undoStack;
    std::vector<EditAction> redoStack;
    int maxStackSize;
};

} // namespace UliCS
