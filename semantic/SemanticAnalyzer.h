//
// Created by Dmitry Smirnov on 21.01.2026.
//

#ifndef SCALA_LEXER_SEMANTICANALYZER_H
#define SCALA_LEXER_SEMANTICANALYZER_H

#include <string>

class TopStatSeqNode;

class SemanticAnalyzer {
public:
    bool analyze(TopStatSeqNode* root);

    void exportMethodsToCSV(const std::string& filename);
};


#endif //SCALA_LEXER_SEMANTICANALYZER_H