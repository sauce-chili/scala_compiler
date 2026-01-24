//
// Created by Dmitry Smirnov on 04.01.2026.
//

#include "BufferedYYLex.h"

void BufferedYYLex::push(const TokenInfo &info) {
    _buffer.push_back(info);
}


int BufferedYYLex::getNextToken() {
    if (_buffer.empty()) {
        return _lex();
    }

    TokenInfo t = _buffer.front(); // КОПИЯ НАХУЙ!!!
    _buffer.pop_front();
    yylval = t.value;
    return t.type;
}
