#ifndef COMPILER_DATATYPE_H
#define COMPILER_DATATYPE_H

#include <iostream>
#include <memory>
#include <vector>
#include "tools.h"
#include "nodes/Node.h"


class SimpleTypeNode;
using namespace std;

#define ARR_SIZE_UNSPECIFIED -1

class DataType {
public:
    enum class Kind {
        Undefined, Unit, Int, Float, Char, Bool, String, Class, Array
    };

    Kind kind = Kind::Undefined;

    // для Class
    // полный путь: scala.collection.mutable.ArrayBuffer
    vector<string> qualId; // хз может нахуй не нужно
    string className = "<unspecified>";

    // тип значений которые хранятся в массиве
    DataType *elementType; // тип элемента, Array[Array[Int]]
    int arrSize = ARR_SIZE_UNSPECIFIED;

    DataType() = default;

    DataType(Kind kind) : kind(kind) {
    };

    bool operator==(const DataType &other) const;

    static DataType makePrimitive(Kind k) {
        return DataType(k);
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

    static DataType makeArray(DataType &elem, int size = ARR_SIZE_UNSPECIFIED) {
        DataType t;
        t.kind = Kind::Array;
        t.elementType = &elem;
        t.arrSize = size;
        return t;
    }

    string toString();

    string toConstTableFormat() const;

    bool isUndefined();

    DataType arrayElementType() const;

    const DataType &deepArrayType() const;

    int arraySize() const;

    bool isPrimitive();

    bool isClass();

    static DataType createFromNode(SimpleTypeNode *node);
};


#endif //COMPILER_DATATYPE_H
