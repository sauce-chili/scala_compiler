#include <string>
#include "vartable.h"

VarTableItem::VarTableItem() {

}


VarTableItem::VarTableItem(string name, DataType dataType, bool isInit, bool isConst) {
    this->id = name;
    this->dataType = dataType;
    this->isInit = isInit;
    this->isConst = isConst;
}

VarTableItem::VarTableItem(string name, DataType dataType, bool isInit, bool isConst,
                           IScope* scope) {
    this->id = name;
    this->dataType = dataType;
    this->isInit = isInit;
    this->isConst = isConst;
}

VarTableItem::VarTableItem(string name, DataType dataType, bool isInit, bool isConst,
                           IScope* scope, ExprNode *value) {
    this->id = name;
    this->dataType = dataType;
    this->isInit = isInit;
    this->isConst = isConst;
    this->value = value;
}

string VarTableItem::toString() {
    string res = "";

    res = this->id;

    if (isConst) {
        res += "val";
    }

    if (isInit) {
        res += "init";
    }

    res += " " + dataType.toString();

    return res;
}

bool VarTableItem::isEquals(const VarTableItem &varTableItem) {

    bool res = true;

    res = res && this->id == varTableItem.id;
    res = res && this->isConst == varTableItem.isConst;
    res = res && this->dataType.isEquals(varTableItem.dataType);
    res = res && this->isInit == varTableItem.isInit;
    res = res && this->value == varTableItem.value;

    return res;
}


string VarTable::toString() {

    string res = "";

    for (int i = 0; i < items.size(); i++) {
        res += to_string(i) + ": " + items[i].toString() + "\n";
    }
    return res;
}

bool VarTable::isExist(const string &varName) {
    bool res = false;

    for (auto elem: this->items) {
        res = res || elem.id == varName;
    }
    return res;
}

bool VarTable::isExist(const string &varName, const ExprNode* expr) {
    bool res = false;

    for (auto i = items.rbegin(); i != items.rend(); ++i) {
        res = res || (i->id == varName); // TODO может надо проверять еще value
        bool f = i->id == varName;
    }
    return res;
}

bool VarTable::isEquals(const VarTable &varTable) {

    bool res = true;
    res = res && this->items.size() == varTable.items.size();
    if (!res) {
        return res;
    }
    for(int i = 0; i < varTable.items.size(); i++){
        res = res && (this->items[i].isEquals(varTable.items[i]));
    }
    return res;
}

VarTableItem VarTable::getVar(int varNum) {
    if (items.size() <= varNum || varNum < 0)
    {
        throw Exception(Exception::NOT_EXIST, "param not exist in method ");

    }

    return items[varNum];
}

int VarTable::getVarNumber(const string &varName, const ExprNode *expr) {

    int cnt = items.size() - 1;
    for (auto i = items.rbegin(); i != items.rend(); ++i) {
        if (i->id == varName){ // TODO может надо проверять еще value
            return cnt;
        }
        cnt--;
    }
    return -1;
}

size_t VarTable::size() const {
    return items.size();
}
