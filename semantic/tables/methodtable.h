#ifndef COMPILER_METHODTABLE_H
#define COMPILER_METHODTABLE_H


#include "../tools/datatype.h"
#include "vartable.h"
#include <map>
#include "constable.h"

using namespace std;

class MethodTableItem {
public:
    DataType returnDataType;
    VarTable paramTable;
    VarTable localVarTable;
    Modifiers* modifiers;

    ExprNode* body;

    MethodTableItem();
    explicit MethodTableItem(const VarTable &paramTable);
    MethodTableItem(DataType returnDataType, VarTable paramTable, VarTable localVarTable, Modifiers* modifiers);
    string toString();
    bool isEqualsDeclaration(const MethodTableItem& other);
    vector<DataType> getParamDataTypes();
    string paramsToConstTableFormat();
    static MethodTableItem initMethod();
    static MethodTableItem clinitMethod();
};


class MethodTable{
public:
    map<string, MethodTableItem> items; // название класса
    string toString();
};

#endif //COMPILER_METHODTABLE_H
