//
// Created by Dmitry Smirnov on 21.01.2026.
//

#ifndef SCALA_LEXER_SEMANTICANALYZER_H
#define SCALA_LEXER_SEMANTICANALYZER_H

#include <string>

class TopStatSeqNode;

class SemanticAnalyzer {
    void linkInheritanceHierarchy();
    void validateInheritanceCycles();
    void validateOverrides();
    void validateAbstractMethods();
    void validateAbstractFields();
    void validateMainMethod();

public:
    bool analyze(TopStatSeqNode* root);
    void exportContext(const std::string& basePath);

    SemanticAnalyzer();
};


#endif //SCALA_LEXER_SEMANTICANALYZER_H