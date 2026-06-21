//token.h contains a huge list of every type of word/symbol that the language should understand.

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType
{
    // Literals
    TOK_NUMBER,
    TOK_STRING,
    TOK_IDENTIFIER,

    // The slang!!
    TOK_WAGWANCUZ, // "wagwan cuz" 1st part of the function definition
    TOK_WYS,       // "wys" 2nd part of the function definition
    TOK_SENDIT,    // "send it" used to return, used for assignment and used to end functions
    TOK_INNITYARA, // "innityara" - semicolon
    TOK_RS3,       // "rs3" - used to declare an if statement
    TOK_M140i,     // "m140i" - used to an else statement
    TOK_REVIT,     // "revit" - used to declare a while loop
    TOK_KASME,     // "kasme" - used to end a if or while
    TOK_GORA,      // "gora" - used to declare a variable
    TOK_NUMBA,     // "numba" - used to declare an integer
    TOK_CHARVA,    // "charva" - used to declare a string
    TOK_WALLAHI,   // "wallahi" - used to declare boolean TRUE
    TOK_BADTAMEEZ, // "badtameez" - used to declare boolean FALSE
    TOK_GEEZER,    // "geezer" - used to declare boolean AND
    TOK_TOPMAN,    // "topman" - used to declare boolean OR
    TOK_LALA,      // "lala" - used to declare boolean NOT
    TOK_MUNCH,     // "munch" - used to declare print

    // Symbols and Operators
    TOK_PLUS,      // +
    TOK_MINUS,     // -
    TOK_MULTIPLY,  // *
    TOK_DIVIDE,    // /
    TOK_MODULO,    // %
    TOK_LESSTHAN,   // benchod (<)
    TOK_GREATERTHAN, // vicked (>)
    TOK_LESSTHANOREQUAL, // benchod= (<=)
    TOK_GREATERTHANOREQUAL, // vicked= (>=)
    TOK_LEFTPARENTHESES, // (
    TOK_RIGHTPARENTHESES, // )
    TOK_COLON, // :
    TOK_COMMA, // ,

    TOK_EOF
};

struct Token
{
    TokenType type;
    std::string text; // The raw text of the token
    double numberValue; // filled in if type is TOK_NUMBER
    int line; // The line number where the token was found 

    Token() : type(TOK_EOF), text(""), numberValue(0), line(0) {}
    Token(TokenType t,std::string txt, int ln): type(t), text(txt), numberValue(0), line(ln) {}

};

#endif
