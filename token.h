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
    TOK_ARRAYLITERAL,

    // The slang!!
    TOK_WAGWANCUZ,      // "wagwan cuz" 1st part of the function definition
    TOK_WYS,            // "wys" 2nd part of the function definition
    TOK_SENDIT,         // "send it" used to return, used for assignment and used to end functions
    TOK_INNITYARA,      // "innityara" - semicolon
    TOK_RS3,            // "rs3" - used to declare an if statement
    TOK_M140i,          // "m140i" - used to an else statement
    TOK_REVIT,          // "revit" - used to declare a while loop
    TOK_KASME,          // "kasme" - used to end an if or while
    TOK_BARETIMES,      // "bare times" - used to declare a for loop
    TOK_GORA,           // "gora" - used to declare a variable
    TOK_NUMBA,          // "numba" - used to declare an integer
    TOK_CHARVA,         // "charva" - used to declare a string
    TOK_OIOIOIOIOI,     // "oioioioioi" - used to declare an array
    TOK_WALLAHI,        // "wallahi" - used to declare boolean TRUE
    TOK_BADTAMEEZ,      // "badtameez" - used to declare boolean FALSE
    TOK_GEEZER,         // "geezer" - used to declare boolean AND
    TOK_TOPMAN,         // "topman" - used to declare boolean OR
    TOK_LALA,           // "lala" - used to declare boolean NOT
    TOK_MUNCH,          // "munch" - used to declare print
    TOK_CUZZY,          // "cuzzy" - used to get input from a user
    TOK_YEMAN,          // "ye man" - used to append a value to an array
    TOK_SAFE,           // "safe" - used to pop off a value of an array and return the value
    TOK_FAKOFF,         // "fak off" - used to delete an item from an array at a given index
    TOK_CHOW,           // "chow" - used to return the size of an array         
    TOK_RAMI,           // "rami"- used to return a character based on a given ASCII code            
    TOK_STAGE3REMAP,    // "stage3remap" - used to return a random number from a given range ie. stage3remap(min,max)
    TOK_PAGGERED,       // "paggered" - used to return CPU time

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
    TOK_LEFTSQUAREBRACKET, // [
    TOK_RIGHTSQUREBRACKET, // ]
    TOK_LEFTCURLYBRACKET, // {
    TOK_RIGHTCURLYBRACKET, // }
    TOK_EQUIVALENT, // ==
    TOK_NOTEQUIVALENT, // != 
    TOK_COLON, // :
    TOK_COMMA, // ,
    TOK_DOT, // .

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
