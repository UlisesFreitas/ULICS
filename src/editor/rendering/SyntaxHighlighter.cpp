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
            layer.Print(comment, currentX, y, LuaSyntax::GetColorForToken(LuaSyntax::TokenType::COMMENT));
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
            layer.Print(str, currentX, y, LuaSyntax::GetColorForToken(LuaSyntax::TokenType::STRING));
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
            layer.Print(num, currentX, y, LuaSyntax::GetColorForToken(LuaSyntax::TokenType::NUMBER));
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
            
            layer.Print(ident, currentX, y, LuaSyntax::GetColorForToken(tokenType));
            currentX += ident.length() * CHAR_W;
            i = identEnd;
            continue;
        }
        
        // Single character (operators, punctuation) - Peach color
        std::string ch(1, c);
        layer.Print(ch, currentX, y, 15);  // UISystem::COLOR_PEACH
        currentX += CHAR_W;
        i++;
    }
}

} // namespace UliCS
