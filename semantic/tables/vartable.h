#ifndef COMPILER_VARTABLE_H
#define COMPILER_VARTABLE_H

#include "../tools/datatype.h"
#include "semantic/scopes/Scope.h"

class ExprNode;

// общая абстракция для:
// FieldVar(поля класса) - всегда имеет дефолтное значение (null если класс, 0 если число)
// LocalVar(это переменные, который создаются в рамках аргументов функций, внутри функция) - обязана иметь значение при инициализации.
class VarTableItem {
public:

    string id = "";
    DataType dataType;
    bool isVal = false;
    bool isInit = false;

    Scope* scope; // область видимости, с которой связана переменная
    ExprNode* value; // для const;

    VarTableItem(); // хуй знает зачем, но пусть

    VarTableItem(string name, DataType dataType, bool isInit, bool isConst);
    VarTableItem(string name, DataType dataType, bool isInit, bool isConst, Scope* scope);
    VarTableItem(string name, DataType dataType, bool isInit, bool isConst, Scope* scope, ExprNode* value);
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
    bool isExist(const string& varName, const Scope* scope);
    int getVarNumber(const string& varName, const Scope* scope);
    VarTableItem getVar(int varNum);
    size_t size() const;
};


#endif //COMPILER_VARTABLE_H
