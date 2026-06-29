//ast.h produces an abstract syntax tree which by using a tree data structure represents the source code using nodes

#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

enum NodeType
{
    //expressions

    NODE_NUMBER,
    NODE_STRING,
    NODE_BOOLEAN,
    NODE_ARRAYLIT,
    NODE_INDEX,
    NODE_INDEX_ASSIGN,
    NODE_ARRAY_APPEND,
    NODE_ARRAY_POP,
    NODE_ARRAY_DELETE,
    NODE_VARIABLE,
    NODE_BINARYOPERATOR,
    NODE_UNARYOPERATOR,
    NODE_CALL,
    NODE_INPUT,
    NODE_SIZE,
    NODE_RANDOM,
    NODE_ASCIICHAR,

    // statements
    NODE_DECLAREVARIABLE,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_FUNCDEF,
    NODE_RETURN,
    NODE_BLOCK,
    NODE_EXPRESSIONSTATEMENT,

    // built in functions
    NODE_APPEND,
    NODE_POP,
    NODE_CPUTIME,

    // SDL2
    NODE_GITTUP,
    NODE_CLOSE,
    NODE_SHOWIT,
    NODE_POLLEVENT,

};

struct Node
{
    NodeType type;
    int line;

    // literals
    double numberValue;
    std::string stringValue;
    bool booleanValue;
    
    // name to be used for variable/function/parameter
    std::string name;

    // children
    Node* left;
    Node* right;
    std::vector<Node*> children;

    // if/while/for
    Node* condition;
    Node* thenBlock;
    Node* elseBlock;

    // function def specific
    std::vector <std::string> parameterNames;

    Node(NodeType t, int ln):
        type(t),
        line(ln),
        numberValue(0),
        booleanValue(false),
        left(nullptr),
        right(nullptr),
        condition(nullptr),
        thenBlock(nullptr),
        elseBlock(nullptr) {}
};


#endif
