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
public:
    bool analyze(TopStatSeqNode* root);

    void exportContextToCSV(const std::string& filename);
    void exportClassesToCSV(const std::string& path);
    void exportMethodsToCSV(const std::string& path);
    void exportFieldsToCSV(const std::string& path);

};


#endif //SCALA_LEXER_SEMANTICANALYZER_H