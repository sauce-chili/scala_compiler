#include <string>
#include "datatype.h"
#include "tools.h"
#include "nodes/type/SimpleTypeNode.h"
#include "semantic/SemanticContext.h"
#include "semantic/tables/tables.hpp"

bool DataType::operator==(const DataType &other) const {
    if (kind != other.kind) return false;

    switch (kind) {
        case Kind::Array:
            return arrSize == other.arrSize &&
                   *this->elementType == *other.elementType;

        case Kind::Class:
            return qualId == other.qualId;

        default:
            return true;
    }
}

string DataType::toString() const {
    switch (kind) {
        case Kind::Int: return "Int";
        case Kind::Any: return "Any";
        case Kind::Double: return "Double";
        case Kind::Bool: return "Bool";
        case Kind::Char: return "Char";
        case Kind::String: return "String";
        case Kind::Unit: return "Unit";
        case Kind::Class: {
            if (qualId.empty()) {
                return className;
            } else {
                string s;
                for (int i = 0; i < qualId.size(); i++) {
                    if (i) s += ".";
                    s += qualId[i];
                }
                return s;
            }
        }
        case Kind::Array: return "Array[" + elementType->toString() + "]";
        case Kind::Undefined: return "<Undefined>";
        case Kind::Null: return "Null";
    }

    return "<error>";
}

string DataType::toConstTableFormat() const {
    return ""; // возможно будет переработан
}

DataType DataType::arrayElementType() const {
    return std::move(*elementType);
}

const DataType &DataType::deepArrayType() const {
    const DataType *current = this;

    while (current->kind == Kind::Array && current->elementType) {
        current = current->elementType;
    }

    return *current;
}

int DataType::arraySize() const {
    return arrSize;
}


bool DataType::isClass() const {
    return kind == Kind::Class;
}

DataType DataType::createFromNode(SimpleTypeNode *typeNode) {
    if (!typeNode) return DataType(Kind::Undefined);

    //  массив Array[T]
    if (typeNode->arrayElemType) {
        DataType innerType = createFromNode(typeNode->arrayElemType);
        return DataType::makeArray(innerType);
    }

    // Это именованный тип (Int, String, MyClass)
    if (typeNode->fullId) {
        std::string name = typeNode->fullId->name;

        if (name == "Int") return DataType::makePrimitive(Kind::Int);
        if (name == "Double") return DataType::makePrimitive(Kind::Double);
        if (name == "Boolean") return DataType::makePrimitive(Kind::Bool);
        if (name == "String") return DataType::makePrimitive(Kind::String);
        if (name == "Char") return DataType::makePrimitive(Kind::Char);
        if (name == "Unit") return DataType::makePrimitive(Kind::Unit);

        // Пользовательский класс
        return DataType::makeClass(name);
    }

    return DataType(Kind::Undefined);
}

bool DataType::isPrimitive() const {
    return kind == Kind::Int || kind == Kind::Double ||
           kind == Kind::Char || kind == Kind::Bool ||
           kind == Kind::String || kind == Kind::Unit;
}

bool DataType::isNull() const {
    return kind == Kind::Null;
}

DataType DataType::makeNull() {
    return DataType(Kind::Null);
}

bool DataType::isAssignableTo(const DataType& target) const {
    // Точное совпадение
    if (*this == target) return true;

    // Any принимает всё
    if (target.kind == Kind::Any) return true;

    // 3. null совместим со ссылочными типами (Class, Array)
    if (this->isNull() && (target.kind == Kind::Class || target.kind == Kind::Array)) {
        return true;
    }

    // Числовое расширение: Int -> Double
    if (target.kind == Kind::Double && this->kind == Kind::Int) {
        return true;
    }

    // Наследование классов
    if (target.kind == Kind::Class && this->kind == Kind::Class) {
        auto targetIt = ctx().classes.find(target.className);
        auto sourceIt = ctx().classes.find(this->className);

        if (targetIt != ctx().classes.end() && sourceIt != ctx().classes.end()) {
            ClassMetaInfo* targetClass = targetIt->second;
            ClassMetaInfo* sourceClass = sourceIt->second;
            return sourceClass->amSubclassOf(targetClass);
        }
    }

    // Ковариантность массивов: Array[Child] -> Array[Parent]
    if (target.kind == Kind::Array && this->kind == Kind::Array) {
        if (this->elementType && target.elementType) {
            return this->elementType->isAssignableTo(*target.elementType);
        }
    }

    return false;
}
