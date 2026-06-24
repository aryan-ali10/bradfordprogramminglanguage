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
        s += advance();
    }
    if (!isAtEnd()) advance();
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

Token Lexer::identifierOrKeyword()
{
    int startLine = line;
    std::string word = readWord();

    // Check if its a combination
    Token multiWordToken = tryMultiWord(word);
    if (multiWordToken.type != TOK_EOF)
    {
        return multiWordToken;
    }

    if (word == "benchod" && peek() == '=')
    {
        advance();
        return Token(TOK_LESSTHANOREQUAL, "benchod=", startLine);
    }

    if (word == "vicked" && peek() == '=')
    {
        advance();
        return Token(TOK_GREATERTHANOREQUAL, "vicked=", startLine);
    }



    // Single word keywords
    if (word == "wys") return Token(TOK_WYS, word, startLine);
    if (word == "rs3") return Token(TOK_RS3, word, startLine);
    if (word == "m140i") return Token(TOK_M140i, word, startLine);
    if (word == "kasme") return Token(TOK_KASME, word, startLine);
    if (word == "gora") return Token(TOK_GORA, word, startLine);
    if (word == "numba") return Token(TOK_NUMBA, word, startLine);
    if (word == "charva") return Token(TOK_CHARVA, word, startLine);
    if (word == "wallahi") return Token(TOK_WALLAHI, word, startLine);  
    if (word == "badtameez") return Token(TOK_BADTAMEEZ, word, startLine);
    if (word == "geezer") return Token(TOK_GEEZER, word, startLine);
    if (word == "lala") return Token(TOK_LALA, word, startLine);
    if (word == "munch") return Token(TOK_MUNCH, word, startLine);
    if (word == "benchod") return Token(TOK_LESSTHAN, word, startLine);
    if (word == "vicked") return Token(TOK_GREATERTHAN, word, startLine);
    
    // If not any of the above then its an identifier for a variable
    return Token(TOK_IDENTIFIER, word, startLine);

}

std::vector<Token> Lexer::tokenise()
{
    std::vector<Token> tokens;
    
    while(true)
    {
        skipWhitespaceAndComments();
        if (isAtEnd())
        {
            break;
        }

        char c = peek();

        if (isdigit(c))
        {
            tokens.push_back(makeNumber());
            continue;
        }
        
        if (c == '"')
        {
            tokens.push_back(makeString());
            continue;
        }

        if (isalpha(c) || c == '_')
        {
            tokens.push_back(identifierOrKeyword());
            continue;
        }

        //operators and symbols
        int startLine = line;
        switch (c)
        {
            case '+':
                advance();
                tokens.push_back(Token(TOK_PLUS, "+", startLine));
                break;
                
            case '-':
                advance();
                tokens.push_back(Token(TOK_MINUS, "-", startLine));
                break;

            case '*':
                advance();
                tokens.push_back(Token(TOK_MULTIPLY, "*", startLine));
                break;

            case '/':
                advance();
                tokens.push_back(Token(TOK_DIVIDE, "/", startLine));
                break;

            case '%':
                advance();
                tokens.push_back(Token(TOK_MODULO, "%", startLine));
                break;

            case '(':
                advance();
                tokens.push_back(Token(TOK_LEFTPARENTHESES, "(", startLine));
                break;

            case ')':
                advance();
                tokens.push_back(Token(TOK_RIGHTPARENTHESES, ")", startLine));
                break;

            case ':':
                advance();
                tokens.push_back(Token(TOK_COLON, ":", startLine));
                break;

            case ',':
                advance();
                tokens.push_back(Token(TOK_COMMA, ",", startLine));
                break;
            
            case '=':
                advance();
                if (peek() == '=')
                {
                    advance();
                    tokens.push_back(Token(TOK_EQUIVALENT, "==", startLine));
                }
                else
                {
                    std::cerr << "kasme yara I have no clue what this symbol is on line: " << line << ": =\n";
                }
                break;

            case '!':
                if (peekNext() == '=')
                {
                    advance();
                    advance();
                    tokens.push_back(Token(TOK_NOTEQUIVALENT, "!=", startLine));
                }
                else
                {
                    advance();
                    std::cerr << "kasme yara I have no clue what this symbol is on line: " << line << ": !\n";
                }
                break;


            default:
                std::cerr << "kasme yara I have no idea what this symbol is on line: " << line << ": " << c << std::endl;
                advance(); // Skip the unexpected character
                break;

        }
        
    }
    tokens.push_back(Token(TOK_EOF,"",line));
    return tokens;

}


