#include "fieldtable.h"

FieldTableItem::FieldTableItem() {
    isConst = false;
}

FieldTableItem::FieldTableItem(DataType dataType, bool isConst, Modifiers *modifiers) {
    this->dataType = dataType;
    this->isConst = isConst;
    this->modifiers = modifiers;
}

FieldTableItem::FieldTableItem(DataType dataType, bool isConst, ExprNode *value, Modifiers *modifiers) {
    this->dataType = dataType;
    this->isConst = isConst;
    this->value = value;
    this->modifiers = modifiers;
}

string FieldTableItem::toString() {
    string res = modifiers->toString();

    if(isConst) {
        res += "const ";
    }

    res += dataType.toString();

    return res;
}

bool FieldTableItem::isEquals(const FieldTableItem &fieldTableItem) {
    bool res = this->isConst == fieldTableItem.isConst;
    res = res && this->isInit == fieldTableItem.isInit;
    res = res && this->dataType.isEquals(fieldTableItem.dataType);
    res = res && this->modifiers->isEquals(*fieldTableItem.modifiers);
    return res;
}

string FieldTable::toString() {
    string res = "";

    for(auto elem: items)
    {
        string tmp = elem.first + ":" + elem.second.toString() + "\n";
        res += tmp;
    }

    return res;
}
