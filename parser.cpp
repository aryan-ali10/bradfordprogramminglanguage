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
    while (!check(TOK_KASME) && !check(TOK_M140i))
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
    if (check(TOK_BARETIMES)) return forStatement();
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

do
{
    if (check(TOK_NUMBA) || check(TOK_CHARVA))
    {
        pos++;
    }
    
    Token param = consume(TOK_IDENTIFIER, "expected parameter name");
    fn->parameterNames.push_back(param.text);

} while (match(TOK_COMMA));
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

Node* Parser:: forStatement()
{
    int ln = current().line;
    consume(TOK_BARETIMES, "expected 'bare times'");
    consume(TOK_LEFTPARENTHESES, "expected '(' after 'bare times'");
    
    consume(TOK_GORA, "expected 'gora' in for loop init");
    if (check(TOK_NUMBA) || check (TOK_CHARVA)) pos++;
    Token nameTok = consume(TOK_IDENTIFIER, "expecyed loop variable name");
    consume(TOK_SENDIT, "expected 'send it' in for loop init");
    Node* initValue = expression();
    Node* initStatement = new Node(NODE_DECLAREVARIABLE, ln);
    initStatement -> name = nameTok.text;
    initStatement -> right = initValue;
    consume(TOK_COMMA, "expected ',' after for loop init");

    Node* cond = expression();
    consume(TOK_COMMA, "expected ',' after for loop condition");

    Token incrementNameTok = consume(TOK_IDENTIFIER, "expected loop variable in increment");
    consume(TOK_SENDIT, "expected 'send it' in for loop increment");
    Node* incrementValue = expression();
    Node* incrementStatement = new Node(NODE_ASSIGN, ln);
    incrementStatement -> name = incrementNameTok.text;
    incrementStatement -> right = incrementValue;

    consume(TOK_RIGHTPARENTHESES, "expected ')' to close for loop header");
    consume(TOK_COLON, "expected ':' after for loop header");

    Node* n = new Node(NODE_FOR, ln);
    n -> left = initStatement;
    n -> condition = cond;
    n -> right = incrementStatement;
    n -> thenBlock = block();

    consume(TOK_KASME, "expected 'kasme' to close 'bare times' block");
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

// munch  (expression) innit yara
Node* Parser::printStatement()
{
    int ln = current().line;
    consume(TOK_MUNCH, "expected 'munch'");
    consume(TOK_LEFTPARENTHESES, "expected '(' after 'munch')");
    Node* val = expression();
    consume(TOK_RIGHTPARENTHESES, "expected ')' after the argument");
    consume(TOK_INNITYARA, "expected 'innit yara' after munch statement");

    Node* n = new Node(NODE_PRINT, ln);
    n-> left = val;
    return n;
}

Node * Parser::returnOrAssignStatement()
{
    int ln = current().line;
    consume(TOK_SENDIT, "expected 'send it'");

    Node* n = new Node(NODE_RETURN, ln);
    if (!check(TOK_INNITYARA))
    {
        n -> left = expression();
    }
    consume(TOK_INNITYARA, "expected 'innit yara' after 'send it'");
    return n;
}

/*
------------expressions------------
precedence in order of low to high:
orExpression (top man)
andExpression (geezer)
notExpression (lala)
comparison (benchod /vicked / benchod= / vicked= /== /!=)
term (+-)
factor (* /)
unary (-)
primary
*/

Node * Parser::expression()
{
    return orExpression();
}


Node* Parser::orExpression()
{
    Node* left = andExpression();
    while (check(TOK_TOPMAN))
    {
        int ln = current().line;
        pos++;
        Node* right = andExpression();
        Node* n = new Node(NODE_BINARYOPERATOR, ln);
        n ->name = "top man";
        n -> left = left;
        n ->right = right;
        left = n;
    }
    return left;
}

Node* Parser::andExpression()
{
    Node* left = notExpression();
    while (check(TOK_GEEZER))
    {
        int ln = current().line;
        pos++;
        Node* right = notExpression();
        Node* n = new Node(NODE_BINARYOPERATOR, ln);
        n -> name = "geezer";
        n -> left = left;
        n -> right = right;
        left = n;
    }
    return left;
}

Node* Parser::notExpression()
{
    if (check(TOK_LALA))
    {
        int ln = current().line;
        pos++;
        Node* operand = notExpression();
        Node* n = new Node(NODE_UNARYOPERATOR, ln);
        n -> name = "lala";
        n -> left = operand;
        return n;
    }
    return equivalence();
}

Node* Parser::equivalence()
{
    Node* left = comparison();

    while (check(TOK_EQUIVALENT) || check(TOK_NOTEQUIVALENT))
    {
        int ln = current().line;
        std::string op = (check(TOK_EQUIVALENT)) ? "==" : "!=";
        pos++;

        Node* right = comparison();

        Node* n = new Node(NODE_BINARYOPERATOR, ln);
        n -> name = op;
        n -> left = left;
        n -> right = right;

        left =n;
    }
    return left;
}

Node* Parser::comparison()
{
    Node* left = term();
    while (check(TOK_LESSTHAN) || check(TOK_GREATERTHAN) || check(TOK_LESSTHANOREQUAL) || check(TOK_GREATERTHANOREQUAL))
    {
        int ln = current().line;
        std::string op;
        if (check(TOK_LESSTHAN)) op = "<";
        else if (check(TOK_LESSTHANOREQUAL)) op = "<=";
        else if (check(TOK_GREATERTHAN)) op = ">";
        else if (check(TOK_GREATERTHANOREQUAL)) op = ">=";
        pos++;
        Node* right = term();
        Node* n = new Node(NODE_BINARYOPERATOR, ln);
        n -> name = op;
        n -> left = left;
        n -> right = right;
        left = n;
    }
    return left;
}

Node* Parser::term()
{
    Node* left = factor();
    while (check(TOK_PLUS) || check(TOK_MINUS))
    {
        int ln = current().line;
        std::string op;
        if (check(TOK_PLUS)) op = "+";
        else if (check(TOK_MINUS)) op = "-";
        pos++;
        Node* right = factor();
        Node* n = new Node(NODE_BINARYOPERATOR, ln);
        n -> name = op;
        n -> left = left;
        n -> right = right;
        left = n;
    }
    return left;
}

Node* Parser::factor()
{
    Node* left = unary();
    while (check(TOK_MULTIPLY) || check(TOK_DIVIDE))
    {
        int ln = current().line;
        std::string op;
        if (check(TOK_MULTIPLY)) op = "*";
        else if (check(TOK_DIVIDE)) op = "/";
        pos++;
        Node* right = unary();
        Node* n = new Node(NODE_BINARYOPERATOR, ln);
        n -> name = op;
        n -> left = left;
        n -> right = right;
        left = n;
    }
    return left;
}

Node* Parser::unary()
{
    if (check(TOK_MINUS))
    {
        int ln = current().line;
        pos++;
        Node* operand = unary();
        Node* n = new Node(NODE_UNARYOPERATOR, ln);
        n -> name = "-";
        n -> left = operand;
        return n;
    }
    return primary();
}


Node* Parser::primary()
{
    int ln = current().line;

    if (check(TOK_NUMBER))
    {
        Node* n = new Node(NODE_NUMBER, ln);
        n -> numberValue = current().numberValue;
        pos++;
        return n;
    }

    if (check(TOK_STRING))
    {
        Node* n = new Node(NODE_STRING, ln);
        n -> stringValue = current().text;
        pos++;
        return n;
    }

    if (check(TOK_CUZZY))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'cuzzy'");

        Node* n = new Node(NODE_INPUT, ln);

        if (!check(TOK_RIGHTPARENTHESES))
        {
            n -> left = expression();
        }

        consume(TOK_RIGHTPARENTHESES, "expected ')' after 'cuzzy' arguments");
        return n;
    }

    if (check(TOK_WALLAHI))
    {
        Node*n = new Node(NODE_BOOLEAN, ln);
        n -> booleanValue = true;
        return n;
    }

    if (check(TOK_BADTAMEEZ))
    {
        Node*n = new Node(NODE_BOOLEAN, ln);
        n -> booleanValue = false;
        return n;
    }

    if (check(TOK_LEFTPARENTHESES))
    {
        pos++;
        Node*e = expression();
        consume(TOK_RIGHTPARENTHESES, "expected ')' to close expression");
        return e;
    }

    if (check(TOK_IDENTIFIER))
    {
            std::string name = current().text;
            pos++;

            if (check(TOK_LEFTPARENTHESES))
            {
                Node* callerNode = new Node(NODE_VARIABLE, ln);
                callerNode->name = name;
                return finishCall(callerNode);
            }
            Node* n = new Node(NODE_VARIABLE, ln);
            n->name = name;
            return n;
    }

    error("expected an expression (number, string, identifier, wallaho/badtameez or parentheses)");
    return nullptr;

}

Node* Parser::finishCall(Node* callerNameNode)
{
    int ln = callerNameNode -> line;
    consume(TOK_LEFTPARENTHESES, "expected '(' for function call");
    Node* call = new Node(NODE_CALL, ln);
    call -> name = callerNameNode -> name;
    delete callerNameNode;

    if (!check(TOK_RIGHTPARENTHESES))
    {
        do
        {
            call -> children.push_back(expression());
        } while (match(TOK_COMMA));
    }
    consume(TOK_RIGHTPARENTHESES, "expected ')' to close function");
    return call;
}
