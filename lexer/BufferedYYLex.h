//
// Created by dima6 on 04.01.2026.
//

#ifndef SCALA_LEXER_BUFFEREDYYLEX_H
#define SCALA_LEXER_BUFFEREDYYLEX_H
#include <deque>
#include <functional>

#include "scala_parser.hpp"

struct TokenInfo {
    int type = YYEMPTY;
    YYSTYPE value;
    int line = -1;
    string tokenName = "NON";
};

class BufferedYYLex {
    std::deque<TokenInfo> _buffer;
    std::function<int()> _lex;

public:
    explicit BufferedYYLex(const std::function<int()> &realLex) : _lex(realLex) {
    }

    // вызываем в собственной обертке yylex
    int getNextToken();

    void push(const TokenInfo& info);
};


#endif //SCALA_LEXER_BUFFEREDYYLEX_H
