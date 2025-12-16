#include "editor/text/TextBuffer.h"
#include <sstream>

namespace UliCS {

const std::string TextBuffer::emptyString = "";

TextBuffer::TextBuffer() {
    // Start with one empty line
    lines.push_back("");
}

TextBuffer::~TextBuffer() {
}

int TextBuffer::GetLineCount() const {
    return static_cast<int>(lines.size());
}

const std::string& TextBuffer::GetLine(int lineIndex) const {
    if (lineIndex < 0 || lineIndex >= static_cast<int>(lines.size())) {
        return emptyString;
    }
    return lines[lineIndex];
}

std::string& TextBuffer::GetLineMutable(int lineIndex) {
    static std::string fallback = "";
    if (lineIndex < 0 || lineIndex >= static_cast<int>(lines.size())) {
        fallback = "";
        return fallback;
    }
    return lines[lineIndex];
}

std::string TextBuffer::GetAllText() const {
    std::stringstream ss;
    for (size_t i = 0; i < lines.size(); i++) {
        ss << lines[i];
        if (i < lines.size() - 1) {
            ss << '\n';
        }
    }
    return ss.str();
}

void TextBuffer::SetAllText(const std::string& text) {
    lines.clear();
    
    std::stringstream ss(text);
    std::string line;
    
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    
    // Ensure at least one line
    if (lines.empty()) {
        lines.push_back("");
    }
}

void TextBuffer::Clear() {
    lines.clear();
    lines.push_back("");
}

bool TextBuffer::InsertChar(int lineIndex, int colIndex, char c) {
    if (lineIndex < 0 || lineIndex >= static_cast<int>(lines.size())) {
        return false;
    }
    
    std::string& line = lines[lineIndex];
    
    // Clamp column to valid range
    if (colIndex < 0) colIndex = 0;
    if (colIndex > static_cast<int>(line.length())) {
        colIndex = line.length();
    }
    
    line.insert(colIndex, 1, c);
    return true;
}

char TextBuffer::DeleteChar(int lineIndex, int colIndex) {
    if (lineIndex < 0 || lineIndex >= static_cast<int>(lines.size())) {
        return '\0';
    }
    
    std::string& line = lines[lineIndex];
    
    if (colIndex < 0 || colIndex >= static_cast<int>(line.length())) {
        return '\0';
    }
    
    char deletedChar = line[colIndex];
    line.erase(colIndex, 1);
    return deletedChar;
}

bool TextBuffer::InsertLine(int lineIndex, int colIndex) {
    if (lineIndex < 0 || lineIndex >= static_cast<int>(lines.size())) {
        return false;
    }
    
    std::string& currentLine = lines[lineIndex];
    
    // Clamp column
    if (colIndex < 0) colIndex = 0;
    if (colIndex > static_cast<int>(currentLine.length())) {
        colIndex = currentLine.length();
    }
    
    // Split line at column
    std::string restOfLine = currentLine.substr(colIndex);
    currentLine = currentLine.substr(0, colIndex);
    
    // Insert new line after current
    lines.insert(lines.begin() + lineIndex + 1, restOfLine);
    
    return true;
}

std::string TextBuffer::DeleteLine(int lineIndex) {
    if (lineIndex < 0 || lineIndex >= static_cast<int>(lines.size())) {
        return "";
    }
    
    // Don't allow deleting the last line - keep buffer non-empty
    if (lines.size() == 1) {
        std::string content = lines[0];
        lines[0] = "";
        return content;
    }
    
    std::string deletedLine = lines[lineIndex];
    lines.erase(lines.begin() + lineIndex);
    
    return deletedLine;
}

int TextBuffer::GetLineLength(int lineIndex) const {
    if (lineIndex < 0 || lineIndex >= static_cast<int>(lines.size())) {
        return 0;
    }
    return static_cast<int>(lines[lineIndex].length());
}

bool TextBuffer::IsEmpty() const {
    return lines.empty() || (lines.size() == 1 && lines[0].empty());
}

void TextBuffer::EnsureNotEmpty() {
    if (lines.empty()) {
        lines.push_back("");
    }
}

} // namespace UliCS
