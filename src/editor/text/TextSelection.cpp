#include "editor/text/TextSelection.h"
#include <algorithm>

namespace UliCS {

TextSelection::TextSelection()
    : active(false)
    , startLine(0)
    , startCol(0)
    , endLine(0)
    , endCol(0) {
}

TextSelection::~TextSelection() {
}

bool TextSelection::IsActive() const {
    return active;
}

void TextSelection::Start(int line, int col) {
    active = true;
    startLine = line;
    startCol = col;
    endLine = line;
    endCol = col;
}

void TextSelection::Update(int line, int col) {
    if (active) {
        endLine = line;
        endCol = col;
    }
}

void TextSelection::Clear() {
    active = false;
}

void TextSelection::Normalize() {
    if (!active) return;
    
    // Swap if start comes after end
    if (startLine > endLine || 
        (startLine == endLine && startCol > endCol)) {
        std::swap(startLine, endLine);
        std::swap(startCol, endCol);
    }
}

void TextSelection::GetNormalizedBounds(int& outStartLine, int& outStartCol,
                                       int& outEndLine, int& outEndCol) const {
    outStartLine = startLine;
    outStartCol = startCol;
    outEndLine = endLine;
    outEndCol = endCol;
    
    // Normalize output
    if (outStartLine > outEndLine ||
        (outStartLine == outEndLine && outStartCol > outEndCol)) {
        std::swap(outStartLine, outEndLine);
        std::swap(outStartCol, outEndCol);
    }
}

void TextSelection::GetRawBounds(int& outStartLine, int& outStartCol,
                                int& outEndLine, int& outEndCol) const {
    outStartLine = startLine;
    outStartCol = startCol;
    outEndLine = endLine;
    outEndCol = endCol;
}

bool TextSelection::ContainsLine(int line) const {
    if (!active) return false;
    
    int minLine = std::min(startLine, endLine);
    int maxLine = std::max(startLine, endLine);
    
    return line >= minLine && line <= maxLine;
}

bool TextSelection::IsEmpty() const {
    return !active || (startLine == endLine && startCol == endCol);
}

} // namespace UliCS
