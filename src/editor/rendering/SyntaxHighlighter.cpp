#include "editor/rendering/SyntaxHighlighter.h"
#include "rendering/AestheticLayer.h"
#include "ui/LuaSyntax.h"

namespace UliCS {

SyntaxHighlighter::SyntaxHighlighter() {
}

SyntaxHighlighter::~SyntaxHighlighter() {
}

void SyntaxHighlighter::RenderLine(const std::string& line, int x, int y, AestheticLayer& layer) {
    int currentX = x;
    size_t i = 0;
    
    while (i < line.length()) {
        char c = line[i];
        
        // Whitespace
        if (LuaSyntax::IsWhitespace(c)) {
            currentX += CHAR_W;
            i++;
            continue;
        }
        
        // Comments (-- to end of line)
        if (c == '-' && i + 1 < line.length() && line[i + 1] == '-') {
            std::string comment = line.substr(i);
            SDL_Color color = LuaSyntax::GetColorForToken(LuaSyntax::TokenType::COMMENT);
            layer.PrintRGB(comment.c_str(), currentX, y, color.r, color.g, color.b);
            break;
        }
        
        // Strings
        if (c == '"' || c == '\'') {
            char quote = c;
            size_t endQuote = i + 1;
            
            while (endQuote < line.length() && line[endQuote] != quote) {
                endQuote++;
            }
            
            if (endQuote < line.length()) {
                endQuote++;
            }
            
            std::string str = line.substr(i, endQuote - i);
            SDL_Color color = LuaSyntax::GetColorForToken(LuaSyntax::TokenType::STRING);
            layer.PrintRGB(str.c_str(), currentX, y, color.r, color.g, color.b);
            currentX += str.length() * CHAR_W;
            i = endQuote;
            continue;
        }
        
        // Numbers
        if (LuaSyntax::IsDigit(c) || (c == '-' && i + 1 < line.length() && LuaSyntax::IsDigit(line[i + 1]))) {
            size_t numEnd = i;
            if (c == '-') numEnd++;
            
            while (numEnd < line.length() && 
                   (LuaSyntax::IsDigit(line[numEnd]) || line[numEnd] == '.')) {
                numEnd++;
            }
            
            std::string num = line.substr(i, numEnd - i);
            SDL_Color color = LuaSyntax::GetColorForToken(LuaSyntax::TokenType::NUMBER);
            layer.PrintRGB(num.c_str(), currentX, y, color.r, color.g, color.b);
            currentX += num.length() * CHAR_W;
            i = numEnd;
            continue;
        }
        
        // Identifiers/Keywords
        if (LuaSyntax::IsIdentifierStart(c)) {
            size_t identEnd = i;
            while (identEnd < line.length() && LuaSyntax::IsIdentifierChar(line[identEnd])) {
                identEnd++;
            }
            
            std::string ident = line.substr(i, identEnd - i);
            LuaSyntax::TokenType tokenType = LuaSyntax::GetTokenType(ident);
            
            SDL_Color color = LuaSyntax::GetColorForToken(tokenType);
            layer.PrintRGB(ident.c_str(), currentX, y, color.r, color.g, color.b);
            currentX += ident.length() * CHAR_W;
            i = identEnd;
            continue;
        }
        
        // Single character (operators, punctuation) - Peach color
        std::string ch(1, c);
        layer.PrintRGB(ch.c_str(), currentX, y, SystemColors::PEACH.r, SystemColors::PEACH.g, SystemColors::PEACH.b);
        currentX += CHAR_W;
        i++;
    }
}

} // namespace UliCS
