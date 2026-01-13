#include "codegenerator.h"
#include "fstream"

const path CodeGenerator::codeGenDir = "./code_generation/";

void CodeGenerator::generate() {

    if (exists(CodeGenerator::codeGenDir)) {
        std::filesystem::remove_all(codeGenDir);
    }

    std::filesystem::create_directory(codeGenDir);
    generateClass("FirstTestClass");
}

void CodeGenerator::generateClass(const std::string &className) {
    std::string fileName = className + ".class";
    path filepath = codeGenDir / fileName;

    create_directories(filepath.parent_path());

    std::ofstream outfile(filepath, std::ios::binary);

    std::vector<char> bytes = { 'T', 'E', 'S', 'T' };
    outfile.write(bytes.data(), bytes.size());
}