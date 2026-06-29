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
    if (check(TOK_CLOSEWINDOW)) return closeWindow();
    if (check(TOK_SHOWIT)) return present();
    if (check(TOK_WIPE)) return clearScreen();
    
    if (check(TOK_IDENTIFIER))
    {
        Node* target = primary();

        if (check(TOK_SENDIT)) // Assignment
        {
            int ln = target ->line;
            pos++;
            Node* value = expression();
            consume(TOK_INNITYARA, "expected 'innit yara' after assignment"); 
            
            if (target -> type == NODE_INDEX)
            {
                Node* n = new Node(NODE_INDEX_ASSIGN, ln);
                n -> left = target;
                n -> right = value;
                return n;
            }
            else
            {
                Node* n = new Node(NODE_ASSIGN, ln);
                n -> left = target;
                n -> right = value;
                return n;
            }
        }

        consume(TOK_INNITYARA, "expected 'innit yara' after expression statement");
        Node* n = new Node(NODE_EXPRESSIONSTATEMENT, target -> line);
        n -> left = target;
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
            if (check(TOK_NUMBA) || check(TOK_CHARVA) || check(TOK_OIOIOIOIOI))
            {
                pos++;
            }

            Token parameter = consume(TOK_IDENTIFIER, "expected parameter name");
            fn -> parameterNames.push_back(parameter.text);
            
        } while (match(TOK_COMMA));
    }
    consume(TOK_RIGHTPARENTHESES, "expected ')' after parameters");
    consume(TOK_COLON, "expected ':' after function");

    fn -> thenBlock = block();

    consume (TOK_KASME, "expected 'kasme' to close the function body");

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
    Token nameTok = consume(TOK_IDENTIFIER, "expected loop variable name");
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
    Node* incrementVar = new Node(NODE_VARIABLE, ln);
    incrementVar -> name = incrementNameTok.text;
    Node* incrementStatement = new Node(NODE_ASSIGN, ln);
    incrementStatement -> left = incrementVar;
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

    if (check(TOK_NUMBA) || check(TOK_CHARVA) || check(TOK_OIOIOIOIOI))
    {
        pos++;
    }

    else
    {
        error("expected 'numba', 'charva' or 'oioioioioi' after 'gora'");
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

Node* Parser::returnOrAssignStatement()
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

Node* Parser::present()
{
    int ln = current().line;
    consume(TOK_SHOWIT, "expected 'showit");
    consume(TOK_INNITYARA, "expected 'innit yara' after show it");
    return new Node(NODE_SHOWIT, ln);
}

Node* Parser::closeWindow()
{
    int ln = current().line;
    consume(TOK_CLOSEWINDOW, "expected 'closeWindow'");
    consume(TOK_INNITYARA, "expected 'innit yara' after closeWindow");
    return new Node(NODE_CLOSE, ln);
}

Node* Parser::clearScreen()
{
    int ln = current().line;
    consume(TOK_WIPE, "expected 'wipe'");
    consume(TOK_INNITYARA, "expected 'innit yara' after wipe");
    return new Node(NODE_WIPE, ln);
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

    if (check(TOK_LEFTSQUAREBRACKET))
    {
        int loopBodyLine = current().line;
        pos++;
        Node* array = new Node(NODE_ARRAYLIT, loopBodyLine);
        if (!check(TOK_RIGHTSQUREBRACKET))
        {
            do
            {
                array -> children.push_back(expression());
            } while (match(TOK_COMMA));
        }
        consume(TOK_RIGHTSQUREBRACKET, "expected ']' to close array literal");
        return array;
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

    if (check(TOK_CHOW))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'chow'");

        Node* n = new Node(NODE_SIZE, ln);
        n -> left = expression();

        consume (TOK_RIGHTPARENTHESES, "expected ')' after argument");
        return n;
    }

    if (check(TOK_STAGE3REMAP))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'stage3remap'");

        Node* n = new Node(NODE_RANDOM, ln);
        n -> left = expression();

        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> right = expression();

        consume(TOK_RIGHTPARENTHESES, "expected ')' after arguments");
        return n;

    }

    if (check(TOK_RAMI))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'rami'");

        Node* n = new Node(NODE_ASCIICHAR, ln);
        n -> left = expression();

        consume(TOK_RIGHTPARENTHESES, "expected ')' after 'rami' argument");
        return n;
    }

    if (check(TOK_GITTUP))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after gittup");
        Node* n = new Node(NODE_GITTUP, ln);
        n -> children.push_back(expression()); // width
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // height
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // title
        consume(TOK_RIGHTPARENTHESES, "expected ')' after  'gittup' arguments");
        return n;
    }

    if (check(TOK_POLLEVENT))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'pollEvent'");
        consume(TOK_RIGHTPARENTHESES, "expected ')'");
        return new Node(NODE_POLLEVENT, ln);
    }

    if (check(TOK_COLOUR))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after colour");
        Node* n = new Node(NODE_COLOUR, ln);
        n -> children.push_back(expression()); // red
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // green
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // blue
        consume(TOK_COMMA, "expected ',' in between arguments");   
        n -> children.push_back(expression()); // alpha
        consume(TOK_RIGHTPARENTHESES, "expected ')' after  'colour' arguments");
        return n;
    }

    if (check(TOK_DRAWPOINT))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'drawPoint'");
        Node* n = new Node(NODE_DRAWPOINT, ln);
        n -> children.push_back(expression());  // x pos
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression());  // y pos
        consume(TOK_RIGHTPARENTHESES, "expected ')' after 'drawPoint' arguments");
        return n;
    }

    if (check(TOK_DRAWLINE))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'drawLine'");
        Node* n = new Node(NODE_DRAWLINE, ln);
        n -> children.push_back(expression()); // x1 pos
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // x2 pos
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // y1 pos
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // y2 pos
        consume(TOK_RIGHTPARENTHESES, "expected ')' after 'drawLine' arguments");
        return n;
    }

    if (check(TOK_DRAWRECTANGLE))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'drawRectangle'");
        Node* n = new Node(NODE_DRAWRECTANGLE, ln);
        n -> children.push_back(expression()); // x pos
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // y pos
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // width
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // height
        consume(TOK_RIGHTPARENTHESES, "expected ')' after 'drawRectangle' arguments");
        return n;
    }

    if (check(TOK_FILLRECTANGLE))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'fillRectangle'");
        Node* n = new Node(NODE_FILLRECTANGLE, ln);
        n -> children.push_back(expression()); // x pos
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // y pos
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // width
        consume(TOK_COMMA, "expected ',' in between arguments");
        n -> children.push_back(expression()); // height
        consume(TOK_RIGHTPARENTHESES, "expected ')' after 'fillRectangle' arguments");
        return n;
    }



    if (check(TOK_PAGGERED))
    {
        pos++;
        consume(TOK_LEFTPARENTHESES, "expected '(' after 'paggered'");
        consume(TOK_RIGHTPARENTHESES, "expected ')' after 'paggered' (takes no arguments)");

        Node* n = new Node(NODE_CPUTIME,ln);
        return n;

    }

    if (check(TOK_WALLAHI))
    {
        Node*n = new Node(NODE_BOOLEAN, ln);
        n -> booleanValue = true;
        pos++;
        return n;
    }

    if (check(TOK_BADTAMEEZ))
    {
        Node*n = new Node(NODE_BOOLEAN, ln);
        n -> booleanValue = false;
        pos++;
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

            while (true)
            {
                if (check(TOK_LEFTSQUAREBRACKET))
                {
                    int indexLine = current().line;
                    pos++;
                    Node* indexExpression = expression();
                    consume(TOK_RIGHTSQUREBRACKET, "expected ']' after array");
                    Node* indexNode = new Node(NODE_INDEX, indexLine);
                    indexNode -> left = n;
                    indexNode -> right = indexExpression;
                    n = indexNode;
                    continue;
                }

                if (check(TOK_YEMAN))
                {
                    int opLine = current().line;
                    pos++;
                    consume(TOK_LEFTPARENTHESES, "expected '(' after 'ye man'");
                    Node* valueExpression = expression();
                    consume(TOK_RIGHTPARENTHESES, "expected ')' to close 'ye man'");
                    Node* appendNode = new Node(NODE_ARRAY_APPEND, opLine);
                    appendNode -> left = n;
                    appendNode -> right = valueExpression;
                    n = appendNode;
                    continue;
                }

                if (check(TOK_SAFE))
                {
                    int opLine = current().line;
                    pos++;
                    Node* popNode = new Node(NODE_ARRAY_POP, opLine);
                    popNode -> left = n;
                    n = popNode;
                    continue;
                }

                if (check(TOK_FAKOFF))
                {
                    int opLine = current().line;
                    pos++;
                    consume(TOK_LEFTPARENTHESES, "expected '(' after 'fak off'");
                    Node* indexExpression = expression();
                    consume(TOK_RIGHTPARENTHESES, "expected ')' to close 'fak off'");
                    Node* deleteNode = new Node(NODE_ARRAY_DELETE, opLine);
                    deleteNode -> left = n;
                    deleteNode -> right = indexExpression;
                    n = deleteNode;
                    continue;
                }
                break;
            }
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
