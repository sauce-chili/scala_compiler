//
// Created by Dmitry Smirnov on 13.01.2026.
//

#ifndef SCALA_LEXER_SCOPE_H
#define SCALA_LEXER_SCOPE_H
#include <optional>
#include <string>

#include "semantic/tables/methodtable.h"
#include "semantic/tables/vartable.h"


class IScope {
public:
    // TODO подумать по какому имени искать про простому или qualId
    std::optional<VarTableItem> findVariable(string varName);

    std::optional<DataType> findType(string name);

    std::optional<MethodTableItem> findMethod(string name);
};

class Scope {
};


#endif //SCALA_LEXER_SCOPE_H
