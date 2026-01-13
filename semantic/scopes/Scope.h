//
// Created by Dmitry Smirnov on 13.01.2026.
//

#ifndef SCALA_LEXER_SCOPE_H
#define SCALA_LEXER_SCOPE_H
#include <optional>
#include <string>

using namespace std;

class MethodTableItem;
class VarTableItem;
class DataType;

class IScope {
public:
    // TODO подумать по какому имени искать про простому или qualId
    static std::optional<VarTableItem> findVariable(const string& varName);

    std::optional<DataType> findType(const string& name);

    std::optional<MethodTableItem> findMethod(const string& name);
};

class Scope {
};


#endif //SCALA_LEXER_SCOPE_H
