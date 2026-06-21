// lexer.cpp A lexer turns raw source code text into a list of tokens that the parser can understand.

#include "lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string & sourceCode) : src(sourceCode), pos(0), line(1) {}

bool Lexer::isAtEnd() 
{
    return pos >= src.length();
}

char Lexer::peek()
{
    if (isAtEnd()) return '\0';
    return src[pos];
}

char Lexer::peekNext() 
{
    if (pos + 1 >= src.size()) return '\0';
    return src[pos + 1];
}

char Lexer::advance()
{
    char c = src[pos];
    pos++;
    if (c == '\n') line++;
    return c;
}

void Lexer::skipWhitespaceAndComments()
{
    while (!isAtEnd())
    {
        char c = peek();

        if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
        {
            advance();
        }

        else if (c == 'm' && src.compare(pos, 4, "mush") == 0)
        {
            char after = (pos +4 < src.size()) ? src[pos+4] : ' ';

            if (!isalnum(after))
            {
                while (!isAtEnd() && peek() != '\n') advance();
            }

            else
            {
                break; //Only happens if its not a comment
            }
        
        }

        else
        {
            break; // If it is neither whitespace or a comment
        }
    }

}
