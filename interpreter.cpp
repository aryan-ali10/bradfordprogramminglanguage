// interpreter.cpp walks the abstract syntax tree

#include "interpreter.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <memory>

Interpreter::Interpreter():globalEnv(nullptr)
{
    std::random_device rd;
    rng.seed(rd());
}

bool Interpreter::truthy(const Value & v) 
{
    if (v.type == VAL_BOOL) return v.booleanValue;
    if (v.type == VAL_NUMBER) return v.numberValue != 0;
    if (v.type == VAL_STRING) return !v.stringValue.empty();
    if (v.type == VAL_ARRAY) return v.arrayValue && !v.arrayValue -> empty();
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
    if (v.type == VAL_ARRAY)
    {
        std::string out = "[";
        if (v.arrayValue)
        {
            for (size_t i = 0; i < v.arrayValue -> size(); i++)
            {
                if (i>0) out += ", ";
                out += toDisplayString((*v.arrayValue)[i]);
            }
            out += "]";
            return out;
        }
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
    startTime = std::chrono::steady_clock::now();

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
            env-> declare(statement -> name,v);
            break;
        }

        case NODE_ASSIGN:
        {
            Value v = eval(statement-> right, env);
            env-> set(statement-> left -> name,v);
            break;
        }

        case NODE_INDEX_ASSIGN:
        {
            size_t index;
            Value & slot = resolveIndexTarget(statement -> left, env, index);
            slot = eval(statement -> right, env);
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

        case NODE_FOR:
        {
            Environment forEnv(env);
            execStatement(statement -> left, &forEnv);

            while (truthy(eval(statement -> condition, &forEnv)))
            {
                Environment loopEnv(&forEnv);
                execBlock(statement -> thenBlock, &loopEnv);
                execStatement(statement -> right, &forEnv);
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

        case NODE_CLOSE:
        {
            if (sdlRenderer) { SDL_DestroyRenderer(sdlRenderer); sdlRenderer = nullptr; }
            if (sdlWindow)   { SDL_DestroyWindow(sdlWindow);     sdlWindow   = nullptr; }
            SDL_Quit();
            break;
        }

        case NODE_SHOWIT:
        {
            if (sdlRenderer) SDL_RenderPresent(sdlRenderer);
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

Value & Interpreter::resolveIndexTarget(Node* indexNode, Environment* env, size_t & outIndex)
{
    if (indexNode -> type != NODE_INDEX)
    {
        std::cerr << "kasme yara, I can only index into an array on line " << indexNode -> line << "\n";
        exit(1);
    }

    Value base;

    if (indexNode -> left -> type == NODE_VARIABLE)
    {
        if (!env -> has (indexNode -> left -> name))
        {
            std::cerr << "kasme yara, what you on about '" << indexNode -> left -> name << "' (line " << indexNode -> line << ")\n";
            exit(1);
        }
        base = env -> get(indexNode -> left -> name);
    }
    else if (indexNode -> left -> type == NODE_INDEX)
    {
        // nested indexes
        size_t innerIndex;
        Value & inner = resolveIndexTarget(indexNode -> left, env, innerIndex);
        base = inner;
    }

    else
    {
        std::cerr << "kasme yara, can't index into that expression (line " << indexNode -> line << ")\n";
        exit(1);
    }

    if (base.type != VAL_ARRAY || !base.arrayValue)
    {
        std::cerr << "kasme yara, that's not an array muppet (line " << indexNode -> line << ")\n";
        exit(1);
    }

    Value indexVal = eval(indexNode -> right, env);
    double indexNum = toNumber(indexVal);
    if (indexNum<0)
    {
        std::cerr << "kasme yara , index can't be negative innit (line " << indexNode->line << ")\n";
        exit(1);
    }

    size_t index = (size_t)indexNum;
    if (index >= base.arrayValue -> size())
    {
        std::cerr << "kasme yara, index " << index << " out of range (size " << base.arrayValue -> size() << ") on line " << indexNode -> line << "\n";
        exit(1);
    }

    outIndex = index;
    return (*base.arrayValue)[index];
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

        case NODE_ARRAYLIT:
        {
            Value v;
            v.type = VAL_ARRAY;
            v.arrayValue = std::make_shared<std::vector<Value>>();
            for (Node* elementExpr : expr -> children)
            {
                v.arrayValue -> push_back(eval(elementExpr, env));
            }
            return v;
        }

        case NODE_INDEX:
        {
            size_t index;
            Value result = resolveIndexTarget(expr, env, index);
            return result;
        }

        case NODE_ARRAY_APPEND:
        {
            if (expr -> left -> type != NODE_VARIABLE)
            {
                std::cerr << "kasme yara, 'ye man' only works on a variable innit (line " << expr -> line << ")\n";
                exit(1);
            }
            Value& arrayRef = env -> getReference(expr -> left -> name);
            if (arrayRef.type != VAL_ARRAY || !arrayRef.arrayValue)
            {
                std::cerr << "kasme yara, 'ye man' only works on an array innit (line " << expr -> line << ")\n";
                exit(1);
            }
            Value valueToAppend = eval(expr -> right, env);
            arrayRef.arrayValue -> push_back(valueToAppend);
            return Value();
        }

        case NODE_ARRAY_POP:
        {
            Value arrayVal = eval(expr -> left, env);
            if (arrayVal.type != VAL_ARRAY || !arrayVal.arrayValue)
            {
                std::cerr << "kasme yara 'safe' only works on an array innit (line " << expr -> line << ")\n";
                exit(1);
            }

            if (arrayVal.arrayValue -> empty())
            {
                std::cerr << "kasme yara, ya can't return a value of an empty array ya muppet (line " << expr -> line << ")\n";
                exit(1);
            }

            Value last = arrayVal.arrayValue -> back();
            arrayVal.arrayValue -> pop_back();
            return last;
        }

        case NODE_ARRAY_DELETE:
        {
            Value arrayVal = eval(expr-> left, env);
            if (arrayVal.type != VAL_ARRAY || !arrayVal.arrayValue)
            {
                std::cerr << "kasme yara 'fak off' only works in an array innit (line " << expr -> line << ")\n";
                exit(1);
            }
            Value indexValue = eval(expr -> right, env);
            double indexNumber = toNumber(indexValue);
            if (indexNumber < 0 || (size_t)indexNumber >= arrayVal.arrayValue -> size())
            {
                std::cerr << "kasme yara, that index isn't even in the array ya muppet (line " << expr -> line << ")\n";
                exit(1);
            }
            arrayVal.arrayValue -> erase(arrayVal.arrayValue -> begin() + (size_t)indexNumber);
            return Value();
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

        case NODE_INPUT:
        {
            if (expr -> left)
            {
                Value promptVal = eval(expr -> left, env);
                std::cout << toDisplayString(promptVal);
            }

            std::string line;
            std:: getline(std::cin, line);

            Value result;
            try
            {
                size_t consumedChars;
                double n = std::stod(line, &consumedChars);

                if (consumedChars == line.size())
                {
                    result.type = VAL_NUMBER;
                    result.numberValue = n;
                }
                else
                {
                    result.type = VAL_STRING;
                    result.stringValue = line;
                }
            }
            catch (const std::exception&)
            {
                result.type = VAL_STRING;
                result.stringValue = line;
            }
            return result;
        }

        case NODE_SIZE:
        {
            Value target = eval(expr -> left, env);
            Value result;
            result.type = VAL_NUMBER;

            if (target.type == VAL_ARRAY)
            {
                result.numberValue = target.arrayValue ? (double)target.arrayValue -> size() : 0;
            }

            else if (target.type == VAL_STRING)
            {
                result.numberValue = (double)target.stringValue.size();
            }

            else
            {
                std::cerr << "kasme yara ya can only find the 'size' of a string or an array";
                exit(1);
            }
            return result;
        }
        
        case NODE_RANDOM:
        {
            Value minVal = eval(expr -> left, env);
            Value maxVal = eval(expr -> right, env);
            double minD = toNumber(minVal);
            double maxD = toNumber(maxVal);

            long long low = (long long)minD;
            long long high = (long long)maxD;
            if (low>high)
            {
                std::cerr << "kasme yara, 'stage3remap' minimum can't be greater than maximum";
                exit(1);
            }
            std::uniform_int_distribution<long long> dist(low,high);
            Value result;
            result.type = VAL_NUMBER;
            result.numberValue = (double)dist(rng);
            return result;
        }

        case NODE_ASCIICHAR:
        {
            Value codeVal = eval(expr -> left, env);
            double codeD = toNumber(codeVal);
            int code = (int)codeD;

            if (code < 0 || code >255)
            {
                std::cerr << "kasme yara, 'rami' wants an ASCII code 0-255 innit (line " << expr -> line << ")\n";
                exit(1);
            }

            Value result;
            result.type = VAL_STRING;
            result.stringValue = std::string(1, (char)code);
            return result;

        }

        case NODE_CPUTIME:
        {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = now - startTime;

            Value result;
            result.type = VAL_NUMBER;
            result.numberValue = elapsed.count();
            return result;
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
                if (toNumber(right) != 0)
                {
                    result.numberValue = toNumber(left) / toNumber(right);
                }
                else if (toNumber(right) == 0)
                {
                    std::cerr << "kasme yara you cant divide by 0 innit";
                    exit(1);
                }
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

            else if (op == "==")
            {
                if(left.type == VAL_STRING && right.type == VAL_STRING)
                {
                    result.type = VAL_BOOL;
                    result.booleanValue = (left.stringValue == right.stringValue);
                }

                else if(left.type == VAL_NUMBER && right.type == VAL_NUMBER)
                {
                    result.type = VAL_BOOL;
                    result.booleanValue = (toNumber(left) == toNumber(right));
                }
                else
                {
                    result.type = VAL_BOOL;
                    result.booleanValue = false;
                }
            }

            else if (op == "!=")
            {
                if (left.type == VAL_STRING && right.type == VAL_STRING)
                {
                    result.type = VAL_BOOL;
                    result.booleanValue = (left.stringValue != right.stringValue);
                }


                else if (left.type == VAL_NUMBER && right.type == VAL_NUMBER)
                {
                    result.type = VAL_BOOL;
                    result.booleanValue = (toNumber(left) != toNumber(right));
                }

                else
                {
                    result.type = VAL_BOOL;
                    result.booleanValue = true;
                }

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

        case NODE_GITTUP:
        {
            int w = (int)toNumber(eval(expr -> children[0], env));
            int h = (int)toNumber(eval(expr -> children[1], env));
            std::string title = toDisplayString(eval(expr -> children[2], env));
            SDL_Init(SDL_INIT_VIDEO);
            sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
            sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
            return Value();
        }

        case NODE_POLLEVENT:
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    Value v;
                    v.type = VAL_BOOL;
                    v.booleanValue = false;
                    return v;
                }
            }
            Value v;
            v.type = VAL_BOOL;
            v.booleanValue = true;
            return v;
        }
        default:
        std::cerr << "kasme yara, I have no clue how to evaluate this expression on line " << expr-> line << "\n";
        exit(1); 
    }
    return Value();
}
