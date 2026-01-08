//
// Created by Dmitry Smirnov on 04.01.2026.
//

#include "BufferedYYLex.h"

void BufferedYYLex::push(int tokenType, const YYSTYPE &value, int line) {
    _buffer.push_back({tokenType, value, line});
}


int BufferedYYLex::getNextToken() {
    if (_buffer.empty()) {
        return _lex();
    }

    auto [type, value, line] = _buffer.front();
    _buffer.pop_front();
    yylval = value;
    return type;
}
