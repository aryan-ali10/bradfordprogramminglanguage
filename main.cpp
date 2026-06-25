#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

std::string readFile(const std::string & path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "kasme yara, I couldn't find the file: " << path << "\n";
        exit(1);
    }
    std::stringstream ss;
    ss <<file.rdbuf();
    return ss.str();
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << "<yourscript.bfd>\n";
        return 1;
    }
    std::string source = readFile(argv[1]);

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenise();

    Parser parser(tokens);
    Node* program = parser.parseProgram();

    Interpreter interpret;
    interpret.run(program);

    return 0;
}
