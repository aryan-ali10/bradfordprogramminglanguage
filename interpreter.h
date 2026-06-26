// interpreter.h Walks the abstract syntax tree


#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <cstdlib>
#include "ast.h"

enum ValueType {VAL_NUMBER, VAL_STRING, VAL_BOOL, VAL_ARRAY};

struct Value
{
    ValueType type;
    double numberValue;
    std::string stringValue;
    bool booleanValue;
    std::shared_ptr<std::vector<Value>> arrayValue;

    Value() : type(VAL_NUMBER), numberValue(0), booleanValue(false), arrayValue(nullptr) {}
};

struct Environment
{
    std::map<std::string, Value> vars;
    Environment* parent;

    Environment(Environment* p) : parent(p) {}

    bool has(const std::string & name)
    {
        if (vars.find(name) != vars.end()) return true;
        if (parent) return parent -> has(name);
        return false;
    }

    Value get(const std::string & name)
    {
        if (vars.find(name) != vars.end()) return vars[name];
        if (parent) return parent -> get(name);
        std::cerr << "kasme yara, what you on about '" << name <<  "' \n";
        exit(1); 
    }
    
    // Create new variable if not found
    void set(const std::string & name, Value v)
    {
        if (vars.find(name) != vars.end())
        {
            vars[name] =v;
            return;
        }

        if (parent && parent -> has(name))
        {
            parent ->set(name, v);
            return;
        }
        vars[name] = v;
    }

    void declare(const std::string & name, Value v)
    {
        vars[name] = v;
    }
};

struct ReturnSignal
{
    Value value;
};

class Interpreter
{
    public:
        Interpreter();
        void run(Node* program);

    private:
        Environment globalEnv;
        std::map <std::string, Node*> functions;

        void execBlock(Node* block, Environment* env);
        void execStatement(Node* statement, Environment* env);
        Value eval(Node* expression, Environment* env);

        Value callFunction(const std::string & name, std::vector<Value> & args, int line);


        bool truthy(const Value & v);
        std::string toDisplayString(const Value & v);
        double toNumber(const Value & v);
        Value& resolveIndexTarget(Node* indexNode, Environment* env, size_t& outIndex);

};

#endif
