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
    int pendingNlCount = 0;
    int pendingSemicolonCount = 0;
    TokenInfo lastToken;
    std::vector<bool> nlEnabledStack;

    std::string removeUnderscore(const char *text);

    long long toInt(const char *text, int base);

    double toDouble(const char *text);

    bool canBeginStmt(int token);

    bool canEndStmt(int token);

    void updateStateByCurrentToken(const TokenInfo &info);

public:
    TokenProcessor(BufferedYYLex &lexer);

    // Вызывается при встрече \n
    void onNewLine();

    void onSemicolon();

    // Основной метод логики
    int onToken(int tokenType);

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
