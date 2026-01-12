#include "methodtable.h"

MethodTableItem::MethodTableItem(const VarTable &paramTable) : paramTable(paramTable) {
    modifiers = new Modifiers();
}

MethodTableItem::MethodTableItem() {
    modifiers = new Modifiers();
}

MethodTableItem::MethodTableItem(DataType returnDataType, VarTable paramTable, VarTable localVarTable, Modifiers* modifiers) {
    this->modifiers = new Modifiers();
    this->returnDataType = returnDataType;
    this->paramTable = paramTable;
    this->localVarTable = localVarTable;
}

string MethodTableItem::toString() {
    string res = "";

    res += "modifiers: " + modifiers->toString() + "\n";
    res += "returnType: " + returnDataType.toString() + "\n";
    res += "paramTable: \n" + paramTable.toString() + "localVarTable\n" + localVarTable.toString() + "\n";
    res += "localVars: \n" + localVarTable.toString() + "\n";

    return res;
}

bool MethodTableItem::isEqualsDeclaration(const MethodTableItem &other) {
    bool res = this->returnDataType.isEquals(other.returnDataType);
    res = res && this->paramTable.isEquals(other.paramTable);
    res = res && this->modifiers->isEquals(*other.modifiers);
    return res;
}

vector<DataType> MethodTableItem::getParamDataTypes() {

    vector<DataType> paramsTypes;
    for(auto & param : this->paramTable.items){
        paramsTypes.push_back(param.dataType);
    }

    return paramsTypes;
}

string MethodTableItem::paramsToConstTableFormat() {
    auto params = this->getParamDataTypes();

    return ConstTable::MethodParam(params, returnDataType);
}

MethodTableItem MethodTableItem::initMethod() {
    MethodTableItem methodTableItem = MethodTableItem();
    methodTableItem.returnDataType = DataType(DataType::void_);
    methodTableItem.modifiers->accessModifier = Modifiers::public_;
    VarTableItem varTableItem;
    varTableItem.dataType = DataType::StructDataType("this_class");
    varTableItem.modifiers->accessModifier = Modifiers::public_;
    varTableItem.id = "self";
    methodTableItem.localVarTable.items.push_back(varTableItem);
    return methodTableItem;
}

MethodTableItem MethodTableItem::clinitMethod() {
    MethodTableItem methodTableItem = MethodTableItem();
    methodTableItem.returnDataType = DataType(DataType::void_);
    return methodTableItem;
}

string MethodTable::toString() {
    string res = "";

    for(auto elem : items)
    {
        string tmp = elem.first + elem.second.toString();
        res += tmp;
    }

    return res;
}
