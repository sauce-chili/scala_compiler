#ifndef COMPILER_FIELDTABLE_H
#define COMPILER_FIELDTABLE_H

#pragma once
#include "../tools/datatype.h"
#include <map>
#include <iostream>

#include "semantic/scopes/Scope.h"

using namespace std;

class  ExprNode;
class FieldTableItem {
public:
    DataType dataType;
    bool isConst = false;
    bool isInit = false;
    Modifiers *modifiers;
    ExprNode *value;
    Scope* scope;

    FieldTableItem();
    FieldTableItem(DataType dataType, bool isConst, Modifiers *modifiers);
    FieldTableItem(DataType dataType, bool isConst, ExprNode *value, Modifiers *modifiers);
    string toString();
    bool isEquals(const FieldTableItem& fieldTableItem);
};


class FieldTable {
public:
    map<string, FieldTableItem> items; // ключ - имя, значение понятно
    string toString();
};


#endif //COMPILER_FIELDTABLE_H
