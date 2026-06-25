//parser.h a parser turns the tokens into a tree of nodes!

#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "token.h"
#include "ast.h"

class Parser
{
    public:
        Parser(const std::vector<Token>& toks);
        Node* parseProgram();

    private:
    std::vector<Token> tokens;
    size_t pos;

    Token& current();
    Token& peekNext();
    bool check(TokenType t);
    bool match(TokenType t);
    Token consume(TokenType t, const std::string &errMsg);
    void error(const std::string & msg);

    Node* statement();
    Node* funcDef();
    Node* ifStatement();
    Node* whileStatement();
    Node* forStatement();
    Node* DeclareVariable();
    Node* printStatement();
    Node* returnOrAssignStatement();
    Node* block();

    Node* expression();
    Node* orExpression();
    Node* andExpression();
    Node* notExpression();
    Node* equivalence();
    Node* comparison();
    Node* term();
    Node* factor();
    Node* unary();
    Node* primary();
    Node* finishCall(Node* callerNameNode);

};

#endif
