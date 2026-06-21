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

std::string Lexer::readWord()
{
    std::string word;
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_'))
    {
        word += advance();
    }
    return word;
}

 Token Lexer::makeNumber()
 {
    std::string numStr;
    int startLine = line;
    while (!isAtEnd() && (isdigit(peek())))
    {
        numStr += advance();
    }
    if (peek() == '.' && isdigit(peekNext()))
    {
        numStr += advance();
        while (!isAtEnd() && isdigit(peek()))
        {
            numStr += advance();
        }
    }
    Token t(TOK_NUMBER, numStr, startLine);
    t.numberValue = std::stod(numStr);
    return t;

 }

 Token Lexer::makeString()
 {
    int startLine = line;
    advance();
    std::string s;
    while (!isAtEnd() && peek() != '"')
    {
        s += advance ();
    }
    if (isAtEnd()) advance;
    return Token(TOK_STRING, s, startLine);
 }

 Token Lexer::tryMultiWord(const std::string & firstWord)
 {
    int startLine = line;
    size_t savedPos = pos;
    int savedLine = line;

    // skip spaces
    size_t lookPos = pos;
    while (lookPos <src.size() && (src[lookPos] == ' ' || src [lookPos] == '\t')) 
    {
        lookPos++;
    }
    size_t wordStart = lookPos;
    while (lookPos < src.size() && (isalnum(src[lookPos]) || src[lookPos] == '_'))
    {
        lookPos++;
    }
    std::string secondWord = src.substr(wordStart, lookPos - wordStart);
    
    // check for token combinations
    if (firstWord == "wagwan" && secondWord == "cuz")
    {
        pos = lookPos;
        return Token(TOK_WAGWANCUZ, "wagwan cuz", startLine);
    }

    if (firstWord == "send" && secondWord == "it")
    {
        pos = lookPos;
        return Token(TOK_SENDIT, "send it", startLine);
    }

    if (firstWord == "innit" && secondWord == "yara")
    {
        pos = lookPos;
        return Token(TOK_INNITYARA, "innit yara", startLine);

    }

    if (firstWord == "rev" && secondWord == "it")
    {
        pos = lookPos;
        return Token(TOK_REVIT, "rev it", startLine);
    }

    if (firstWord == "top" && secondWord == "man")
    {
        pos = lookPos;
        return Token(TOK_TOPMAN, "top man", startLine);
    }

    // If no combinations are found then treat it as a single word token
    pos = savedPos;
    line = savedLine;
    Token none;
    none.type = TOK_EOF;
    return none;

 }

