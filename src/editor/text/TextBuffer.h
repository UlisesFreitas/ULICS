#pragma once

#include <string>
#include <vector>

namespace UliCS {

/**
 * @brief Manages text content as a vector of lines
 * 
 * Provides core text manipulation operations: insert, delete, get/set text.
 * This is the foundation for all text editing operations.
 */
class TextBuffer {
public:
    TextBuffer();
    ~TextBuffer();
    
    /**
     * @brief Get total number of lines
     */
    int GetLineCount() const;
    
    /**
     * @brief Get a specific line
     * @param lineIndex Line index (0-based)
     * @return Reference to the line, or empty string if out of bounds
     */
    const std::string& GetLine(int lineIndex) const;
    
    /**
     * @brief Get mutable reference to a line
     */
    std::string& GetLineMutable(int lineIndex);
    
    /**
     * @brief Get all text as single string (with newlines)
     */
    std::string GetAllText() const;
    
    /**
     * @brief Set all text from string (splits by newlines)
     */
    void SetAllText(const std::string& text);
    
    /**
     * @brief Clear all content
     */
    void Clear();
    
    /**
     * @brief Insert a character at specified position
     * @param lineIndex Line to insert at
     * @param colIndex Column to insert at
     * @param c Character to insert
     * @return true if successful
     */
    bool InsertChar(int lineIndex, int colIndex, char c);
    
    /**
     * @brief Delete a character at specified position
     * @param lineIndex Line to delete from
     * @param colIndex Column to delete at
     * @return The deleted character, or '\0' if failed
     */
    char DeleteChar(int lineIndex, int colIndex);
    
    /**
     * @brief Insert a new line at specified position
     * Splits the line at colIndex
     * @param lineIndex Line to split
     * @param colIndex Column to split at
     * @return true if successful
     */
    bool InsertLine(int lineIndex, int colIndex);
    
    /**
     * @brief Delete a line
     * @param lineIndex Line to delete
     * @return The deleted line content, or empty string if failed
     */
    std::string DeleteLine(int lineIndex);
    
    /**
     * @brief Get length of a specific line
     */
    int GetLineLength(int lineIndex) const;
    
    /**
     * @brief Check if buffer is empty
     */
    bool IsEmpty() const;
    
    /**
     * @brief Ensure buffer has at least one line
     */
    void EnsureNotEmpty();

private:
    std::vector<std::string> lines;
    static const std::string emptyString;
};

} // namespace UliCS
