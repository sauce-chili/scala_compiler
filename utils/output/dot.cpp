#include <iostream>
#include <fstream>


void createDotTree(TopStatSeqNode* root, const std::string& filename) {
    if (root == nullptr) {
        std::cerr << "Error: root == nullptr\n";
        return;
    }

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }

    out << root->toDot();
    out.close();

    std::cout << "AST saved to " << filename << "\n";
}