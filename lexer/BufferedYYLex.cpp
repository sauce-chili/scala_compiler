//
// Created by Dmitry Smirnov on 04.01.2026.
//

#include "BufferedYYLex.h"

void BufferedYYLex::push(int tokenType, const YYSTYPE &value, int line) {
    buffer.push_back({tokenType, value, line});
}


int BufferedYYLex::yylex() {
    if (buffer.empty()) {
        return lex();
    }

    auto [type, value, line] = buffer.front();
    buffer.pop_front();
    yylval = value;
    return type;
}
