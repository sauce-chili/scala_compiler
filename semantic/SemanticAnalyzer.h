//
// Created by Dmitry Smirnov on 21.01.2026.
//

#ifndef SCALA_LEXER_SEMANTICANALYZER_H
#define SCALA_LEXER_SEMANTICANALYZER_H

#include <string>
#include <optional>

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

    /**
     * Compile the AST to a JAR file
     *
     * Performs semantic analysis, generates .class files for all user classes,
     * and packages them into a JAR file.
     *
     * @param root The root of the AST
     * @param outputDir Directory for .class files and output JAR
     * @param rtlJarPath Path to the RTL jar file (for Class-Path in manifest)
     * @return Path to the created JAR file, or std::nullopt on failure
     */
    std::optional<std::string> compile(
        TopStatSeqNode* root,
        const std::string& outputDir = "out_jar",
        const std::string& rtlJarPath = "rtl.jar"
    );

    SemanticAnalyzer();
};


#endif //SCALA_LEXER_SEMANTICANALYZER_H