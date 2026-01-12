#ifndef COMPILER_VARTABLE_H
#define COMPILER_VARTABLE_H

#include "../tools/datatype.h"

class ExprNode;

class VarTableItem {
public:

    string id = "";
    DataType dataType;
    bool isConst = false;
    bool isInit = false;
    Modifiers *modifiers;

    ExprNode* expr; // блок, с которым связана переменная
    ExprNode* value; // для const;

    VarTableItem();
    VarTableItem(string name, DataType dataType, bool isInit, bool isConst, Modifiers *modifiers);
    VarTableItem(string name, DataType dataType, bool isInit, bool isConst, ExprNode* expr, Modifiers *modifiers);
    VarTableItem(string name, DataType dataType, bool isInit, bool isConst, ExprNode* expr, ExprNode* value, Modifiers *modifiers);
    VarTableItem(DataType dataType, bool isInit, Modifiers *modifiers) ;
    VarTableItem(DataType dataType, bool isInit, ExprNode *expr, Modifiers *modifiers);
    string toString();

    bool isEquals(const VarTableItem &varTableItem);
};

class VarTable {
public:
    vector<VarTableItem> items;
    string toString();
    bool isExist(const string& varName);
    bool isEquals(const VarTable & varTable);
    bool isExist(const string& varName, const ExprNode* blockExpr);
    int getVarNumber(const string& varName, const ExprNode* blockExpr);
    VarTableItem getVar(int varNum);
    size_t size() const;
};


#endif //COMPILER_VARTABLE_H
