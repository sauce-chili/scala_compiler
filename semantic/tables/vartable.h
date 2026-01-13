#ifndef COMPILER_VARTABLE_H
#define COMPILER_VARTABLE_H

#include "../tools/datatype.h"
#include "semantic/scopes/Scope.h"

class ExprNode;
class IScope;

// общая абстракция для:
// FieldVar(поля класса) - всегда имеет дефолтное значение (null если класс, 0 если число)
// LocalVar(это переменные, который создаются в рамках аргументов функций, внутри функция) - обязана иметь значение при инициализации.
class VarTableItem {
public:

    string id = "";
    DataType dataType;
    bool isConst = false;
    bool isInit = false;

    IScope* scope; // блок, с которым связана переменная
    ExprNode* value; // для const;

    VarTableItem(); // хуй знает зачем, но пусть

    VarTableItem(string name, DataType dataType, bool isInit, bool isConst);
    VarTableItem(string name, DataType dataType, bool isInit, bool isConst, IScope* scope);
    VarTableItem(string name, DataType dataType, bool isInit, bool isConst, IScope* scope, ExprNode* value);
    string toString();

    bool isEquals(const VarTableItem &varTableItem);
};

//  общая абстракция для таблиц локальных переменных и полей классов
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
