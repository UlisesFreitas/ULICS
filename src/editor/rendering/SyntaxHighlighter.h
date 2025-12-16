#pragma once

#include <string>

// Forward declarations
class AestheticLayer;

namespace UliCS {

/**
 * @brief Handles Lua syntax highlighting for code editor
 * 
 * Renders a line of code with proper color coding for Lua keywords,
 * strings, comments, numbers, and identifiers.
 */
class SyntaxHighlighter {
public:
    SyntaxHighlighter();
    ~SyntaxHighlighter();
    
    /**
     * @brief Render a line with syntax highlighting
     * @param line The text line to render
     * @param x X position to start rendering
     * @param y Y position to render at
     * @param layer AestheticLayer to render on
     */
    void RenderLine(const std::string& line, int x, int y, AestheticLayer& layer);
    
private:
    // Character and line dimensions (matching CodeEditor)
    static constexpr int CHAR_W = 8;
    static constexpr int CHAR_H = 8;
};

} // namespace UliCS
