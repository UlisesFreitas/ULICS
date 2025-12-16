#pragma once

#include "../rendering/AestheticLayer.h"
#include "../ui/UISystem.h"
#include "../editor/text/TextBuffer.h"
#include "../editor/text/TextSelection.h"
#include "../editor/rendering/SyntaxHighlighter.h"
#include <string>

namespace UliCS {

/**
 * @brief Handles all rendering for the code editor
 * 
 * Responsible for drawing text lines, cursor, line numbers, messages, and selection.
 * Coordinates with SyntaxHighlighter for syntax coloring.
 */
class EditorRenderer {
public:
    EditorRenderer();
    ~EditorRenderer();
    
    /**
     * @brief Render the complete editor
     * @param layer Drawing layer
     * @param ui UI system for text rendering
     * @param textBuffer Text content to render
     * @param selection Current selection state
     * @param cursorLine Current cursor line
     * @param cursorCol Current cursor column
     * @param scrollY Vertical scroll position
     * @param scrollX Horizontal scroll position
     * @param savedMessageTimer Timer for "saved" message (0 = not shown)
     * @param reloadedMessageTimer Timer for "reloaded" message (0 = not shown)
     */
    void Render(
        AestheticLayer& layer,
        UISystem& ui,
        const TextBuffer& textBuffer,
        const TextSelection& selection,
        int cursorLine,
        int cursorCol,
        int scrollY,
        int scrollX,
        int savedMessageTimer,
        int reloadedMessageTimer
    );
    
    /**
     * @brief Set the current syntax highlighter
     */
    void SetSyntaxHighlighter(SyntaxHighlighter* highlighter);
    
private:
    SyntaxHighlighter* syntaxHighlighter;
    
    // Layout constants
    static constexpr int LINE_NUM_WIDTH = 30;
    static constexpr int TEXT_X = LINE_NUM_WIDTH + 5;
    static constexpr int EDITOR_TOP = 2;
    static constexpr int EDITOR_BOTTOM = 236;
    static constexpr int CHAR_W = 6;
    static constexpr int CHAR_H = 8;
    static constexpr int LINE_HEIGHT = 10;
    static constexpr int VISIBLE_LINES = (EDITOR_BOTTOM - EDITOR_TOP) / LINE_HEIGHT;
    static constexpr int VISIBLE_COLS = (252 - TEXT_X) / CHAR_W;
};

} // namespace UliCS
