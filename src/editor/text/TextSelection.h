#pragma once

namespace UliCS {

/**
 * @brief Manages text selection state for editors
 * 
 * Tracks selection start/end positions and provides utilities
 * for normalizing and querying selection state.
 */
class TextSelection {
public:
    TextSelection();
    ~TextSelection();
    
    /**
     * @brief Check if selection is active
     */
    bool IsActive() const;
    
    /**
     * @brief Start a new selection at given position
     */
    void Start(int line, int col);
    
    /**
     * @brief Update selection end position
     */
    void Update(int line, int col);
    
    /**
     * @brief Clear the selection
     */
    void Clear();
    
    /**
     * @brief Normalize selection so start comes before end
     */
    void Normalize();
    
    /**
     * @brief Get selection bounds (normalized)
     * @param outStartLine Output: start line
     * @param outStartCol Output: start column
     * @param outEndLine Output: end line
     * @param outEndCol Output: end column
     */
    void GetNormalizedBounds(int& outStartLine, int& outStartCol, 
                            int& outEndLine, int& outEndCol) const;
    
    /**
     * @brief Get raw selection bounds (not normalized)
     */
    void GetRawBounds(int& outStartLine, int& outStartCol,
                     int& outEndLine, int& outEndCol) const;
    
    /**
     * @brief Check if a given line is within selection
     * @param line Line number to check
     * @return true if line is part of selection
     */
    bool ContainsLine(int line) const;
    
    /**
     * @brief Check if selection is empty (start == end)
     */
    bool IsEmpty() const;

private:
    bool active;
    int startLine;
    int startCol;
    int endLine;
    int endCol;
};

} // namespace UliCS
