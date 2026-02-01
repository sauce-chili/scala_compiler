#ifndef COMPILER_DATATYPE_H
#define COMPILER_DATATYPE_H

#include <vector>
#include <optional>
#include "nodes/Node.h"


class SimpleTypeNode;
class SimpleExpr1Node;
using namespace std;

#define ARR_SIZE_UNSPECIFIED (-1)

class DataType {
public:
    enum class Kind {
        Undefined, Unit, Int, Double, Char, Bool, String, Class, Array, Any, Null
    };

    Kind kind = Kind::Undefined; // НЕЛЬЗЯ использовать за пределами этого класса

    // для Class
    // полный путь: scala.collection.mutable.ArrayBuffer
    vector<string> qualId; // хз может нахуй не нужно
    string className = "<unspecified>";

    // тип значений которые хранятся в массиве
    DataType *elementType = nullptr; // тип элемента, Array[Array[Int]]
    int arrSize = ARR_SIZE_UNSPECIFIED;

    DataType() = default;

    DataType(Kind kind) : kind(kind) {}

    DataType(const DataType &other)
        : kind(other.kind),
          qualId(other.qualId),
          className(other.className),
          elementType(other.elementType ? new DataType(*other.elementType) : nullptr),
          arrSize(other.arrSize) {}

    DataType &operator=(const DataType &other) {
        if (this != &other) {
            delete elementType;
            kind = other.kind;
            qualId = other.qualId;
            className = other.className;
            elementType = other.elementType ? new DataType(*other.elementType) : nullptr;
            arrSize = other.arrSize;
        }
        return *this;
    }

    ~DataType() {
        delete elementType;
    }

    bool operator==(const DataType &other) const;

    static DataType makePrimitive(Kind k) {
        return DataType(k);
    }

    static DataType makeInt() {
        return DataType(Kind::Int);
    }

    static DataType makeDouble() {
        return DataType(Kind::Double);
    }

    static DataType makeBoolean() {
        return DataType(Kind::Bool);
    }

    static DataType makeChar() {
        return DataType(Kind::Char);
    }

    static DataType makeString() {
        return DataType(Kind::String);
    }

    static DataType makeUnit() {
        return DataType(Kind::Unit);
    }

    static DataType makeClass(const vector<string> &qid) {
        DataType t;
        t.kind = Kind::Class;
        t.qualId = qid;
        t.className = qid.back();
        return t;
    }

    static DataType makeClass(string className) {
        DataType t;
        t.kind = Kind::Class;
        t.className = className;
        return t;
    }

    static DataType makeArray(const DataType &elem, int size = ARR_SIZE_UNSPECIFIED) {
        DataType t;
        t.kind = Kind::Array;
        t.elementType = new DataType(elem);
        t.arrSize = size;
        return t;
    }

    string toString() const;

    string toConstTableFormat() const;

    DataType arrayElementType() const;

    const DataType &deepArrayType() const;

    int arraySize() const;

    bool isPrimitive() const;

    bool isClass() const;

    bool isNull() const;

    bool isUndefined() const;

    string getClassName() const;

    /**
     * Возвращает JVM-дескриптор типа.
     * Примеры: I (Int), D (Double), Ljava/lang/String; (String), [I (Array[Int])
     */
    string toJvmDescriptor() const;

    /**
     * Проверяет можно ли присвоить значение данного типа переменной целевого типа.
     * Учитывает: точное совпадение, Any принимает всё, null совместим со ссылочными типами,
     * числовое расширение (Int -> Double), наследование классов, ковариантность массивов.
     */
    bool isAssignableTo(const DataType& target) const;

    /**
     * Находит ближайший общий предок (LCA) двух типов.
     * Используется для определения типа if-else выражений.
     * Возвращает std::nullopt если общий тип не существует.
     */
    static std::optional<DataType> findCommonAncestor(const DataType& t1, const DataType& t2);

    static DataType createFromNode(SimpleTypeNode *node);

    static string literalClassName(SimpleExpr1Node* literal);

    static std::optional<DataType> primitiveFromName(const string& name);

    static DataType makeNull();
};


#endif //COMPILER_DATATYPE_H
