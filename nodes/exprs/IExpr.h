//
// Created by Dmitry Smirnov on 13.01.2026.
//

#ifndef SCALA_LEXER_IEXPR_H
#define SCALA_LEXER_IEXPR_H
#include "semantic/tools/datatype.h"

class IExpr {
public:
    virtual DataType getDataType() = 0;
    virtual getExprType() = 0;
};

#endif //SCALA_LEXER_IEXPR_H
