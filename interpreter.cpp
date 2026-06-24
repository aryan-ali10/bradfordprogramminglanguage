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

Value Interpreter::callFunction(const std::string &name, std::vector<Value> & args, int line)
{
    if (functions.find(name) == functions.end())
    {
        std::cerr << "nah yara, kasme no function has been called innit: " << name << "' (line" << line << ")\n";
        exit(1);
    }

    Node* fn = functions[name];

    if (args.size() != fn->parameterNames.size())
    {
        std::cerr << "kasme yara '" << name << "' wants" << fn->parameterNames.size() << " args but got" << args.size() << " (line " << line << ")\n";
        exit(1);
    }

    Environment fnEnv(&globalEnv);
    for (size_t i = 0; i < args.size(); i++)
    {
        fnEnv.declare(fn->parameterNames[i], args[i]);
    }

    try
    {
        execBlock(fn ->thenBlock, & fnEnv);
    }

    catch(ReturnSignal & sig)
    {
        return sig.value;

    }
    return Value();

}

Value Interpreter::eval(Node* expr, Environment* env)
{
    switch(expr -> type)
    {
        case NODE_NUMBER:
        {
            Value v;
            v.type = VAL_NUMBER;
            v.numberValue = expr-> numberValue;
            return v;
        }

        case NODE_STRING:
        {
            Value v;
            v.type = VAL_STRING;
            v.stringValue = expr -> stringValue;
            return v;
        }

        case NODE_BOOLEAN:
        {
            Value v;
            v.type = VAL_BOOL;
            v.booleanValue = expr -> booleanValue;
            return v;
        }

        case NODE_VARIABLE:
        {
            return env -> get(expr -> name);
        }

        case NODE_CALL:
        {
            std::vector<Value> args;
            for (Node* argExpr: expr-> children)
            {
                args.push_back(eval(argExpr, env));
            }
            return callFunction(expr->name, args, expr->line);
        }

        case NODE_UNARYOPERATOR:
        {
            Value operand = eval(expr -> left, env);
            Value result;
            if (expr->name == "-")
            {
                result.type = VAL_NUMBER;
                result.numberValue = -toNumber(operand);
            }
            else if (expr -> name == "lala")
            {
                result.type = VAL_BOOL;
                result.booleanValue = !truthy(operand);
            }
            return result;
        }

        case NODE_BINARYOPERATOR:
        {
            Value left = eval(expr -> left, env);
            Value right = eval(expr -> right, env);
            Value result;

            std::string op = expr->name;

            if (op == "+")
            {
                if (left.type == VAL_STRING || right.type == VAL_STRING)
                {
                    result.type = VAL_STRING;
                    result.stringValue = toDisplayString(left) + toDisplayString(right);
                }
                else
                {
                    result.type = VAL_NUMBER;
                    result.numberValue = toNumber(left) + toNumber(right);
                }
            }

            else if (op == "-")
            {
                result.type = VAL_NUMBER;
                result.numberValue = toNumber(left) - toNumber(right);
            }

            else if (op == "*")
            {
                result.type = VAL_NUMBER;
                result.numberValue = toNumber(left) * toNumber(right);
            }

             else if (op == "/")
            {
                result.type = VAL_NUMBER;
                result.numberValue = toNumber(left) / toNumber(right);
            }

            else if (op == "<")
            {
                result.type = VAL_BOOL;
                result.booleanValue = toNumber(left) < toNumber(right);
            }

            else if (op == ">")
            {
                result.type = VAL_BOOL;
                result.booleanValue = toNumber(left) > toNumber(right);
            }

            else if (op == ">=")
            {
                result.type = VAL_BOOL;
                result.booleanValue = toNumber(left) >= toNumber(right);
            }

            else if (op == "<=")
            {
                result.type = VAL_BOOL;
                result.booleanValue = toNumber(left) <= toNumber(right);
            }

            else if (op == "geezer")
            {
                result.type = VAL_BOOL;
                result.booleanValue = truthy(left) && truthy(right);
            }

            else if (op == "top man")
            {
                result.type = VAL_BOOL;
                result.booleanValue = truthy(left) || truthy(right);
            }
            
            return result;
        }
        default:
        std::cerr << "kasme yara, I have no clue how to evaluate this expression on line " << expr-> line << "\n";
        exit(1); 
    }
    return Value();
}
