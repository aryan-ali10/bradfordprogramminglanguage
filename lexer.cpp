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
