#include <iostream>
#include <cstdio>
#include <string>

// Подключаем определения токенов, сгенерированные Bison,
// чтобы в режиме лексера можно было (опционально) понимать ID токенов.

// Определяем указатель на корневой узел AST (тип должен совпадать с тем, что в %parse-param)
class ScalaFileNode;

// Функции и переменные из сгенерированных файлов
extern int yyparse(ScalaFileNode** out_root);
extern int yylex();
extern FILE* yyin;

// Переменная отладки Bison (становится доступной благодаря %define parse.trace или %debug)
extern int yydebug;

void print_help(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " <filename>\n";
    std::cout << "Environment variables:\n";
    std::cout << "  SCALA_MODE  : 'lexer' (only tokens) or 'parser' (default)\n";
    std::cout << "  SCALA_DEBUG : '1' to enable verbose debug output\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return -1;
    }

    // 1. Открываем файл
    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        return -1;
    }
    yyin = input_file;

    // 2. Читаем переменные окружения
    const char* mode_env = std::getenv("SCALA_MODE");
    const char* debug_env = std::getenv("SCALA_DEBUG");

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

            // Вывод: ID токена
            // В идеале можно сделать switch-case для красивого вывода имен токенов,
            // но для базовой проверки достаточно ID.
            std::cout << "Token ID: " << token << std::endl;
        }
    } else {
        std::cout << "--- Running in PARSER mode ---" << std::endl;
        ScalaFileNode* root = nullptr;

        int result = yyparse(&root);

        if (result == 0) {
            std::cout << "Parsing completed successfully!" << std::endl;
            if (root) {
                std::cout << "AST Root created at address: " << root << std::endl;
                // Здесь можно вызвать root->print() или подобный метод, если он есть
            }
        } else {
            std::cerr << "Parsing failed." << std::endl;
        }
    }

    fclose(input_file);
    return 0;
}