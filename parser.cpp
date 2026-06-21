//parser.cpp A parser turns the tokens into a tree of nodes!

#include "parser.h"
#include <iostream>
#include <cstdlib>

Parser::Parser(const std::vector<Token>&toks): tokens(toks), pos(0) {}

Token & Parser::current()
 {
    return tokens[pos];
 }

Token & Parser::peekNext()
 {
    if (pos + 1 < tokens.size()) return tokens[pos+1];
    return tokens[tokens.size()-1];
 }

bool Parser::check(TokenType t)
 {
    return current().type == t;
 }

bool Parser::match(TokenType t)
 {
    if (check(t))
    {
        pos++;
        return true;
    }
    return false;
 }

Token Parser::consume(TokenType t, const std::string & errorMsg)
 {
    if (check(t))
    {
        Token tok = current();
        pos++;
        return tok;
    }

    error(errorMsg);
    return current();

 }

void Parser::error(const std::string& msg) {
    std::cerr << "kasme yara ya got a parse error on line " << current().line << ": " << msg
              << " (got '" << current().text << "')\n";
    exit(1);
}

Node* Parser::parseProgram()
{
    Node* prog = new Node(NODE_BLOCK, 1);
    while (!check(TOK_EOF))
    {
        prog -> children.push_back(statement());
    }
    return prog;
}

Node* Parser::block()
{
    Node* b = new Node(NODE_BLOCK, current().line);
    while (!check(TOK_KASME) && !check(TOK_M140i) && !check(TOK_IDENTIFIER))
    {
        if (check(TOK_SENDIT))
        {
            TokenType after = tokens[pos+1].type;
            bool looksLikeReturn = (after == TOK_INNITYARA) ||
                after == TOK_NUMBER || after == TOK_STRING || after == TOK_IDENTIFIER ||
                after == TOK_WALLAHI || after == TOK_BADTAMEEZ || after == TOK_LEFTPARENTHESES ||
                after == TOK_MINUS || after == TOK_LALA;
            if (!looksLikeReturn)
            {
                break;
            }

        }
        
        b->children.push_back(statement());
    
    }
    return b;
}


Node* Parser::statement()
{
    if (check(TOK_WAGWANCUZ)) return funcDef();
    if (check(TOK_RS3)) return ifStatement();
    if (check(TOK_REVIT)) return whileStatement();
    if (check(TOK_GORA)) return DeclareVariable();
    if (check(TOK_MUNCH)) return printStatement();
    if (check(TOK_SENDIT)) return returnOrAssignStatement();
    
    if (check(TOK_IDENTIFIER) && peekNext().type == TOK_SENDIT)
    {
        int ln = current().line;
        std::string varName = current().text;
        pos++;
        pos++;
        Node* val  = expression();
        consume(TOK_INNITYARA, "expected 'innit yara' after assignment");
        Node* n = new Node(NODE_ASSIGN, ln);
        n -> name = varName;
        n -> right = val;
        return n;
    }

    int ln = current().line;
    Node*e = expression();
    consume(TOK_INNITYARA, "expected 'innit yara' after expression statement");
    Node* n = new Node(NODE_EXPRESSIONSTATEMENT, ln);
    n -> left = e;
    return n;
}

// wagwan cuz wys
Node* Parser::funcDef()
{
    int ln = current().line;
    consume(TOK_WAGWANCUZ, "expected 'wagwan cuz'");
    consume(TOK_WYS, "expected 'wys' after wagwan cuz");
    Token nameTok = consume(TOK_IDENTIFIER, "expected function name after wys");

    consume(TOK_LEFTPARENTHESES, "expected '(' after function name");
    Node* fn = new Node(NODE_FUNCDEF, ln);
    fn -> name = nameTok.text;

    if (!check(TOK_RIGHTPARENTHESES))
    {
        do
        {
            if (check(TOK_NUMBA) || check(TOK_CHARVA))
            {
                pos++;
            }

        } while (match(TOK_COMMA));
    }
    consume(TOK_RIGHTPARENTHESES, "expected ')' after parameters");
    consume(TOK_COLON, "expected ':' after function");

    fn -> thenBlock = block();

    consume (TOK_SENDIT, "expected 'send it' to close the function body");

    return fn;

}

// rs3 (condition) : block [m140i:block] kasme
Node* Parser::ifStatement()
{
    int ln = current().line;
    consume(TOK_RS3, "expected 'rs3'");
    consume(TOK_LEFTPARENTHESES, "expected '(' after rs3");
    Node* cond = expression();
    consume(TOK_RIGHTPARENTHESES, "expected ')' after condition");
    consume(TOK_COLON, "expected ':' after 'rs3' condition");

    Node*n  = new Node(NODE_IF, ln);
    n-> condition = cond;
    n-> thenBlock = block();

    if (match(TOK_M140i))
    {
        consume(TOK_COLON,"expected ':' after m140i");
        n->elseBlock = block();
    }
    
    consume(TOK_KASME, "expected 'kasme' to close 'rs3' block");
    return n;

}

// rev it (condition) : block kasme
Node* Parser:: whileStatement()
{
    int ln = current().line;
    consume(TOK_REVIT, "expected 'rev it'");
    consume(TOK_LEFTPARENTHESES, "expected '(' after 'rev it'");
    Node* cond = expression();
    consume(TOK_RIGHTPARENTHESES, "expected ')' after condition");
    consume(TOK_COLON, "expected ':' after 'rev it' condition");

    Node* n = new Node(NODE_WHILE, ln);
    n -> condition = cond;
    n-> thenBlock = block();

    consume(TOK_KASME, "expected 'kasme' to close 'rev it' block");
    return n;

}

//gora numba x 
Node* Parser::DeclareVariable()
{
    int ln = current().line;
    consume(TOK_GORA, "expected 'gora'");

    if (check(TOK_NUMBA) || check(TOK_CHARVA))
    {
        pos++;
    }

    else
    {
        error("expected 'numba' or 'charva' after 'gora'");
    }

    Token nameTok = consume(TOK_IDENTIFIER, "expected variable name");
    consume(TOK_SENDIT, "expected 'send it' after variable name");
    Node* val = expression();
    consume(TOK_INNITYARA, "expected 'innit yara' after variable declaration");

    Node* n = new Node(NODE_DECLAREVARIABLE, ln);
    n -> name = nameTok.text;
    n-> right = val;
    return n;
}

