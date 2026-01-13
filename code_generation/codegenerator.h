#ifndef SCALA_CODEGENERATOR_H
#define COMPILER_CODEGENERATOR_H
#include "command.h"
#include "tools/utils.h"
#include "tools/byte_convert.h"

#include <filesystem>

using namespace std::filesystem;

class CodeGenerator {
private:

    static const path codeGenDir;
    void generateClass(const std::string &className);

public:
    void generate();
};



#endif //SCALA_CODEGENERATOR_H

