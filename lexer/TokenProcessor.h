//
// Created by Dmitry Smirnov on 05.01.2026.
//

#ifndef SCALA_LEXER_TOKENPROCESSOR_H
#define SCALA_LEXER_TOKENPROCESSOR_H
#include <string>

#include "BufferedYYLex.h"

class TokenProcessor {
    BufferedYYLex &bufferedLexer;

    // состояния для обработки NL токенов
    int pending_nl_count = 0;
    TokenInfo last_token;
    std::vector<bool> nl_enabled_stack;

    std::string remove_underscore(const char *text);

    long long to_int(const char *text, int base);

    double to_double(const char *text);

    bool can_begin_stmt(int token);

    bool can_end_stmt(int token);

    void update_state_by_current_token(const TokenInfo& info);

public:
    TokenProcessor(BufferedYYLex &lexer);

    // Вызывается при встрече \n
    void onNewLine();

    // Основной метод логики
    int onToken(int tokenType); // Перегрузка, если текст не нужен или уже в yylval

    int onDecimalLiteral(const char *text);
    int onHexLiteral(const char *text);
    int onBinLiteral(const char *text);
    int onDoubleLiteral(const char *text);
    int onBooleanLiteral(int token, bool value);
    int onStringLiteral(const std::string &text);
    int onChar(char c);

    int onID(const char *text);
};


#endif //SCALA_LEXER_TOKENPROCESSOR_H
