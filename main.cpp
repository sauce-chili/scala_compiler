#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>

#include "nodes/stats/TopStatSeqNode.h"
#include "semantic/SemanticAnalyzer.h"
#include "semantic/SemanticContext.h"
#include "utils/output/dot.cpp"
#include "semantic/error/SemanticError.h"

// Подключаем определения токенов, сгенерированные Bison,
// чтобы в режиме лексера можно было (опционально) понимать ID токенов.


// Функции и переменные из сгенерированных файлов

int yyparse(TopStatSeqNode **root);

int yylex();

extern "C" {
extern FILE *real_in;
extern int real_lineno;
}

extern int yydebug;

extern const char *get_bison_token_name(int token);

void yyerror(TopStatSeqNode **out_root, const char *s) {
    // Можно использовать внешнюю переменную yylineno, если она объявлена
    extern int real_lineno;
    cerr << "Parser error at line " << real_lineno << ": " << s << endl;
}

void print_help(const char *prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <filename>\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --compile-only    Compile to JAR without running\n";
    std::cout << "  --run             Compile and run the JAR (default)\n";
    std::cout << "  --analyze-only    Only perform semantic analysis (no codegen)\n";
    std::cout << "  --rtl-jar <path>  Path to RTL jar file (default: rtl.jar)\n";
    std::cout << "  --output <dir>    Output directory for JAR (default: out_jar)\n";
    std::cout << "\nEnvironment variables:\n";
    std::cout << "  SCALA_MODE  : 'lexer' (only tokens) or 'parser' (default)\n";
    std::cout << "  SCALA_DEBUG : '1' to enable verbose debug output\n";
}

int runCompile(TopStatSeqNode *root, bool runAfterCompile, bool analyzeOnly,
               const std::string& rtlJarPath, const std::string& outputDir);

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return -1;
    }

    // Parse command line arguments
    bool compileOnly = false;
    bool runAfterCompile = true;
    bool analyzeOnly = false;
    std::string rtlJarPath = "rtl.jar";
    std::string outputDir = "out_jar";
    std::string inputFile;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--compile-only") == 0) {
            compileOnly = true;
            runAfterCompile = false;
        } else if (std::strcmp(argv[i], "--run") == 0) {
            runAfterCompile = true;
        } else if (std::strcmp(argv[i], "--analyze-only") == 0) {
            analyzeOnly = true;
        } else if (std::strcmp(argv[i], "--rtl-jar") == 0 && i + 1 < argc) {
            rtlJarPath = argv[++i];
        } else if (std::strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            outputDir = argv[++i];
        } else if (argv[i][0] != '-') {
            inputFile = argv[i];
        }
    }

    if (inputFile.empty()) {
        print_help(argv[0]);
        return -1;
    }

    // 1. Открываем файл
    FILE *input_file = fopen(inputFile.c_str(), "r");
    if (!input_file) {
        std::cerr << "Cannot open file: " << inputFile << std::endl;
        return -1;
    }
    real_in = input_file;

    // 2. Читаем переменные окружения
    const char *mode_env = std::getenv("SCALA_MODE");
    const char *debug_env = std::getenv("SCALA_DEBUG");

    std::string mode = (mode_env) ? std::string(mode_env) : "parser";
    bool debug = (debug_env && std::string(debug_env) == "1");

    // 3. Настройка отладки
    if (debug) {
        std::cout << "[DEBUG MODE ENABLED]" << std::endl;
        yydebug = 1; // Включает трассировку Bison
    } else {
        yydebug = 0;
    }

    // 4. Запуск в нужном режиме
    if (mode == "lexer") {
        std::cout << "--- Running in LEXER mode ---" << std::endl;
        while (true) {
            int token = yylex();
            if (token == 0) break; // 0 означает EOF
            // Вывод строка | имя токена(ELSE, +, IF и тд), ID токена
            std::cout << real_lineno << " | " << "Token: " << get_bison_token_name(token) << " (" << token << ")" <<
                    std::endl;
        }
    } else {
        std::cout << "--- Running in PARSER mode ---" << std::endl;
        // Scala file
        TopStatSeqNode *root = nullptr;

        int result = yyparse(&root);

        if (result == 0) {
            std::cout << "Parsing completed successfully!" << std::endl;
            if (root) {
                std::cout << "AST Root created at address: " << root << std::endl;
            } else {
                std::cout << "AST Root not created at address: " << root << std::endl;
            }
            return runCompile(root, runAfterCompile, analyzeOnly, rtlJarPath, outputDir);
        } else {
            std::cerr << "Parsing failed." << std::endl;
            fclose(input_file);
            return 1;
        }
    }

    fclose(input_file);
    return 0;
}


int runCompile(TopStatSeqNode *root, bool runAfterCompile, bool analyzeOnly,
               const std::string& rtlJarPath, const std::string& outputDir) {
    SemanticAnalyzer analyzer;

    try {
        root->convertAst();
        createDotTree(root, "after_transform.txt");
    } catch (SemanticError &e) {
        std::cerr << e.getErrorMessage() << std::endl;
        return 1;
    }

    if (analyzeOnly) {
        if (analyzer.analyze(root)) {
            std::cout << "Semantic analysis completed successfully." << std::endl;
            return 0;
        }
        return 1;
    }
    auto jarPathOpt = analyzer.compile(root, outputDir, rtlJarPath);

    if (!jarPathOpt.has_value()) {
        std::cerr << "Compilation failed." << std::endl;
        return 1;
    }

    std::string jarPath = jarPathOpt.value();
    std::cout << "Compilation successful: " << jarPath << std::endl;

    if (runAfterCompile) {
        std::cout << "Running JAR..." << std::endl;
        std::cout << "\n\n\n";
        std::cout << "--------------------------------------------------------------------------\n\n";
        std::string mainClassName = ctx().mainClass ? ctx().mainClass->name : "Main";
        std::string cmd =
                "java -noverify -cp \"" + jarPath + ";" + rtlJarPath + "\" A";
        int exitCode = std::system(cmd.c_str());
        std::cout << "\n--------------------------------------------------------------------------";
        return exitCode;
    }

    return 0;
}
