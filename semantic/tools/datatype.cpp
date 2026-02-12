#include <string>
#include <unordered_set>
#include "datatype.h"
#include "tools.h"
#include "nodes/type/SimpleTypeNode.h"
#include "nodes/exprs/SimpleExpr1Node.h"
#include "semantic/SemanticContext.h"
#include "semantic/tables/tables.hpp"

bool DataType::operator==(const DataType &other) const {
    if (kind != other.kind) return false;

    switch (kind) {
        case Kind::Array:
            return arrSize == other.arrSize &&
                   *this->elementType == *other.elementType;

        case Kind::Class:
            return className == other.className;

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

string DataType::literalClassName(SimpleExpr1Node *literal) {
    switch (literal->type) {
        case _INTEGER_LITERAL: return "Int";
        case _DOUBLE_LITERAL: return "Double";
        case _STRING_LITERAL: return "String";
        case _CHAR_LITERAL: return "Char";
        case _BOOL_LITERAL: return "Boolean";
        default: return "";
    }
}

std::optional<DataType> DataType::primitiveFromName(const string &name) {
    if (name == "Int") return makePrimitive(Kind::Int);
    if (name == "Double") return makePrimitive(Kind::Double);
    if (name == "Boolean") return makePrimitive(Kind::Bool);
    if (name == "String") return makePrimitive(Kind::String);
    if (name == "Char") return makePrimitive(Kind::Char);
    if (name == "Unit") return makePrimitive(Kind::Unit);
    return std::nullopt;
}

string DataType::getClassName() const {
    switch (kind) {
        case Kind::Int: return "Int";
        case Kind::Double: return "Double";
        case Kind::Bool: return "Boolean";
        case Kind::String: return "String";
        case Kind::Char: return "Char";
        case Kind::Unit: return "Unit";
        case Kind::Class: return className;
        case Kind::Array: return "Array";
        case Kind::Any: return "Any";
        case Kind::Null: return "Null";
        default: return "";
    }
}

bool DataType::isPrimitive() const {
    return kind == Kind::Int || kind == Kind::Double ||
           kind == Kind::Char || kind == Kind::Bool ||
           kind == Kind::String || kind == Kind::Unit;
}

bool DataType::isNull() const {
    return kind == Kind::Null;
}

bool DataType::isUndefined() const {
    return kind == Kind::Undefined;
}

string DataType::toJvmDescriptor() const {
    switch (kind) {
        case Kind::Int:     return "Lrtl/Int;";
        case Kind::Double:  return "Lrtl/Double;";  // Double реализуется через float
        case Kind::Char:    return "Lrtl/Char;";
        case Kind::Bool:    return "Lrtl/Boolean;";
        case Kind::Unit:    return "V"; // Чтоб поддерживать Lrtl/Unit; нужно return (empty) преобразовать в return new Unit()
        case Kind::String:  return "Lrtl/String;";
        case Kind::Class:   {
            if (className == "StdIn" || className == "Predef" || className == "Any")
                return "Lrtl/" + className + ";";
            else
                return "L" + className + ";"; }
        case Kind::Array:
            if (elementType) {
                return "[" + elementType->toJvmDescriptor();
            }
            return "[Ljava/lang/Object;";
        case Kind::Any:     return "Lrtl/Any;";
        case Kind::Null:    return "Ljava/lang/Object;";
        default:            return "V";
    }
}

string DataType::toConstructorArgDescriptor(string className) const {
    if (kind == DataType::Kind::Char && className == "Char") {
        return "C";
    } else if (kind == DataType::Kind::Bool && className == "Boolean") {
        return "Z";
    } else if (kind == DataType::Kind::Int && className == "Int") {
        return "I";
    } else if (kind == DataType::Kind::Double && className == "Double") {
        return "F";
    } else if (kind == DataType::Kind::Unit && className == "Unit") {
        return "V";
    } else if (kind == DataType::Kind::String && className == "String") {
        return "Ljava/lang/String;";
    }

    return toJvmDescriptor();
}

DataType DataType::makeNull() {
    return DataType(Kind::Null);
}

bool DataType::isAssignableTo(const DataType &target) const {
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
            ClassMetaInfo *targetClass = targetIt->second;
            ClassMetaInfo *sourceClass = sourceIt->second;
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

std::optional<DataType> DataType::findCommonAncestor(const DataType &t1, const DataType &t2) {
    // Точное совпадение
    if (t1 == t2) return t1;

    // Any принимает всё
    if (t1.kind == Kind::Any || t2.kind == Kind::Any) {
        return DataType::makePrimitive(Kind::Any);
    }

    // Null совместим со ссылочными типами
    if (t1.isNull() && (t2.kind == Kind::Class || t2.kind == Kind::Array)) {
        return t2;
    }
    if (t2.isNull() && (t1.kind == Kind::Class || t1.kind == Kind::Array)) {
        return t1;
    }

    // Оба Null
    if (t1.isNull() && t2.isNull()) {
        return t1;
    }

    // Примитивные типы
    if (t1.isPrimitive() && t2.isPrimitive()) {
        // Числовое расширение: Int и Double -> Double
        if ((t1.kind == Kind::Int && t2.kind == Kind::Double) ||
            (t1.kind == Kind::Double && t2.kind == Kind::Int)) {
            return DataType::makeDouble();
        }
        // Другие примитивы несовместимы
        return std::nullopt;
    }

    // Оба класса - ищем LCA по иерархии
    if (t1.kind == Kind::Class && t2.kind == Kind::Class) {
        auto class1It = ctx().classes.find(t1.className);
        auto class2It = ctx().classes.find(t2.className);

        if (class1It == ctx().classes.end() || class2It == ctx().classes.end()) {
            return std::nullopt;
        }

        ClassMetaInfo *class1 = class1It->second;
        ClassMetaInfo *class2 = class2It->second;

        // Собираем всех предков первого класса (включая его самого)
        std::unordered_set<ClassMetaInfo *> ancestors1;
        ClassMetaInfo *current = class1;
        while (current) {
            ancestors1.insert(current);
            current = current->parent;
        }

        // Поднимаемся по иерархии второго класса, пока не найдем общего предка
        current = class2;
        while (current) {
            if (ancestors1.find(current) != ancestors1.end()) {
                // Нашли LCA
                return DataType::makeClass(current->name);
            }
            current = current->parent;
        }

        // Нет общего предка
        return std::nullopt;
    }

    // Оба массива - проверяем элементы
    if (t1.kind == Kind::Array && t2.kind == Kind::Array) {
        if (t1.elementType && t2.elementType) {
            auto elemLCA = findCommonAncestor(*t1.elementType, *t2.elementType);
            if (elemLCA.has_value()) {
                return DataType::makeArray(elemLCA.value());
            }
        }
        return std::nullopt;
    }

    // Разные категории типов - несовместимы
    return std::nullopt;
}
