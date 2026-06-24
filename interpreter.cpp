// interpreter.cpp walks the abstract syntax tree

#include "interpreter.h"
#include <iostream>
#include <cstdlib>

Interpreter::Interpreter():globalEnv(nullptr) {}

bool Interpreter::truthy(const Value & v) 
{
    if (v.type == VAL_BOOL) return v.booleanValue;
    if (v.type == VAL_NUMBER) return v.numberValue != 0;
    if (v.type == VAL_STRING) return !v.stringValue.empty();
    return false;
}

std::string Interpreter::toDisplayString(const Value & v)
{
    if (v.type == VAL_STRING) return v.stringValue;
    if (v.type == VAL_BOOL) return v.booleanValue ? "wallahi" : "badtameez";
    if (v.type == VAL_NUMBER) 
    {
        double n = v.numberValue;
        if (n==(long long)n)
        {
            return std::to_string((long long)n);
        }
        return std::to_string(n);
    }
    return "";
}

double Interpreter::toNumber(const Value & v)
{
    if (v.type == VAL_NUMBER) return v.numberValue;
    if (v.type ==VAL_BOOL)return v.booleanValue ? 1 : 0;
    return 0;
}

void Interpreter::run(Node* program)
{
    for (Node* statement : program -> children)
    {
        if (statement -> type == NODE_FUNCDEF)
        {
            functions[statement -> name] = statement;
        }
    }

    for (Node* statement : program -> children)
    {
        if (statement -> type != NODE_FUNCDEF)
        {
            execStatement(statement, &globalEnv);
        }
    }
}

void Interpreter::execBlock(Node* block, Environment* env)
{
    for (Node* statement: block -> children)
    {
        execStatement(statement, env);
    }
}


// statements

void Interpreter::execStatement(Node* statement, Environment* env)
{
    switch (statement->type)
    {
        case NODE_DECLAREVARIABLE:
        {
            Value v = eval(statement-> right, env);
            env-> set(statement->name,v);
            break;
        }

        case NODE_ASSIGN:
        {
            Value v = eval(statement-> right, env);
            env-> set(statement->name,v);
            break;
        }

        case NODE_PRINT:
        {
            Value v = eval(statement->left, env);
            std::cout << toDisplayString(v) << "\n";
            break;
        }

        case NODE_IF:
        {
            Value cond = eval(statement-> condition, env);
            if (truthy(cond))
            {
                Environment ifEnv(env);
                execBlock(statement -> thenBlock, &ifEnv);
            }
            else if (statement-> elseBlock)
            {
                Environment elseEnv(env);
                execBlock(statement->elseBlock, &elseEnv);
            }
            break;
        }

        case NODE_WHILE:
        {
            while (truthy(eval(statement->condition, env)))
            {
                Environment loopEnv(env);
                execBlock(statement ->thenBlock, &loopEnv);
            }
            break;
        }

        case NODE_RETURN:
        {
            ReturnSignal sig;
            if (statement->left)
            {
                sig.value = eval(statement->left, env);
            }
            else
            {
                sig.value = Value(); //returns 0 because "send it" with nothing returns 0
            }
            throw sig; // unwind back to callFunction()
        }

        case NODE_EXPRESSIONSTATEMENT:
        {
            eval(statement-> left, env);
            break;
        }

        case NODE_FUNCDEF:
        {
            functions[statement -> name] = statement;
            break;
        }

        default:
            std::cerr <<"kasme yara, what ya on about on line " << statement->line << ", dunno how to execute this statement type\n";
            exit(1);
    }
}
