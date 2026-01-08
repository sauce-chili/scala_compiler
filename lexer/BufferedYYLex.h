//
// Created by dima6 on 04.01.2026.
//

#ifndef SCALA_LEXER_BUFFEREDYYLEX_H
#define SCALA_LEXER_BUFFEREDYYLEX_H
#include <deque>
#include <functional>

#include "scala_parser.hpp"

struct TokenWrap {
    int type;
    YYSTYPE value;
    int line;
};

class BufferedYYLex {
    std::deque<TokenWrap> _buffer;
    std::function<int()> _lex;

public:
    explicit BufferedYYLex(const std::function<int()> &realLex) : _lex(realLex) {
    }

    // вызываем в собственной обертке yylex
    int getNextToken();

    void push(int tokenType, const YYSTYPE &value, int line);
};


#endif //SCALA_LEXER_BUFFEREDYYLEX_H
