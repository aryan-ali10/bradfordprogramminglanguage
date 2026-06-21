//lexer.h A lexer turns raw source code text into a list of tokens that the parser can understand.

#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include "token.h"

class Lexer
{
    public:
        Lexer(const std::string & sourceCode);
        std::vector<Token> tokenise();

    private:
        std::string src;
        size_t pos;
        int line;

        char peek();
        char peekNext();
        char advance();
        bool isAtEnd();
        void skipWhitespaceAndComments();
        
        std::string readWord(); // Reads a continuous line of letters/digits
        Token makeNumber();
        Token makeString();
        Token identifierOrKeyword();

        Token tryMultiWord(const std::string & firstWord); // Starting from the current word, peeks aheead to see if it can form a multi-word token.

};


#endif
