#ifndef LUASYNTAX_H
#define LUASYNTAX_H

#include <string>
#include <unordered_set>
#include <vector>
#include "ui/SystemColors.h"  // Fixed UI colors
#include <SDL.h>  // For SDL_Color

/**
 * @brief LuaSyntax - Lua syntax highlighting definitions
 * 
 * Simple token-based syntax highlighting for Lua code.
 * Uses string matching and regex-like pattern detection.
 * 
 * Colors (PICO-8 inspired):
 * - Keywords: Cyan (12)
 * - Strings: Pink (14)
 * - Numbers: Orange (9)
 * - Comments: Gray (6)
 * - Default: White (7)
 */

namespace LuaSyntax {

// ============================================
// KEYWORD DEFINITIONS
// ============================================

// Lua keywords (reserved words)
const std::unordered_set<std::string> KEYWORDS = {
    // Control flow
    "if", "then", "else", "elseif", "end",
    "for", "while", "do", "repeat", "until",
    "break", "return",
    
    // Functions
    "function", "local",
    
    // Logical
    "and", "or", "not",
    
    // Values
    "true", "false", "nil",
    
    // Other
    "in"
};

// Built-in Lua functions (common ones)
const std::unordered_set<std::string> BUILTINS = {
    // ULICS API (most common)
    "cls", "print", "pset", "line", "rect", "rectfill",
    "circ", "circfill", "spr", "map", "mget", "mset",
    "btn", "btnp", "sfx", "music",
    "sin", "cos", "atan2", "sqrt", "abs", "flr", "ceil", "rnd",
    "time", "camera",
    
    // Lua standard
    "pairs", "ipairs", "next", "type", "tonumber", "tostring",
    "print", "error", "assert", "require",
    "math", "string", "table"
};

// ============================================
// TOKEN TYPES
// ============================================

enum class TokenType {
    KEYWORD,      // if, function, etc.
    BUILTIN,      // print, cls, etc.
    STRING,       // "text" or 'text'
    NUMBER,       // 123, 0.5, -10
    COMMENT,      // -- comment
    IDENTIFIER,   // variable names
    OPERATOR,     // +, -, *, /, etc.
    DEFAULT       // everything else
};

// ============================================
// TOKEN COLORS
// ============================================

inline SDL_Color GetColorForToken(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD:
            return SystemColors::BLUE;      // Blue - Keywords stand out
        case TokenType::BUILTIN:
            return SystemColors::GREEN;     // Green - ULICS/Lua functions
        case TokenType::STRING:
            return SystemColors::PINK;      // Pink - Strings
        case TokenType::NUMBER:
            return SystemColors::ORANGE;    // Orange - Numbers
        case TokenType::COMMENT:
            return SystemColors::DARK_GRAY; // Gray - Comments
        case TokenType::OPERATOR:
            return SystemColors::PEACH;     // Peach - Operators
        case TokenType::IDENTIFIER:
        case TokenType::DEFAULT:
        default:
            return SystemColors::WHITE;     // White - Default
    }
}

// ============================================
// HELPER FUNCTIONS
// ============================================

/**
 * @brief Check if character is a valid identifier start (a-z, A-Z, _)
 */
inline bool IsIdentifierStart(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/**
 * @brief Check if character is a valid identifier continuation (a-z, A-Z, 0-9, _)
 */
inline bool IsIdentifierChar(char c) {
    return IsIdentifierStart(c) || (c >= '0' && c <= '9');
}

/**
 * @brief Check if character is a digit
 */
inline bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

/**
 * @brief Check if character is whitespace
 */
inline bool IsWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

/**
 * @brief Check if string is a keyword
 */
inline bool IsKeyword(const std::string& word) {
    return KEYWORDS.find(word) != KEYWORDS.end();
}

/**
 * @brief Check if string is a built-in function
 */
inline bool IsBuiltin(const std::string& word) {
    return BUILTINS.find(word) != BUILTINS.end();
}

/**
 * @brief Get token type for a word
 */
inline TokenType GetTokenType(const std::string& word) {
    if (IsKeyword(word)) {
        return TokenType::KEYWORD;
    }
    if (IsBuiltin(word)) {
        return TokenType::BUILTIN;
    }
    return TokenType::IDENTIFIER;
}

} // namespace LuaSyntax

#endif // LUASYNTAX_H
