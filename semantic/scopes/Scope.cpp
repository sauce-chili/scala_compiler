//
// Created by Dmitry Smirnov on 13.01.2026.
//

#include "Scope.h"
#include "../tables/methodtable.h"
#include "../tables/vartable.h"

std::optional<MethodTableItem> IScope::findMethod(const string& name) {

}

std::optional<VarTableItem> IScope::findVariable(const string& varName) {
    return std::optional<VarTableItem>();
}

std::optional<DataType> IScope::findType(const string& name) {
    return std::optional<DataType>();
}


