//
// Created by Dmitry Smirnov on 17.01.2026.
//

#ifndef SCALA_LEXER_TABLES_H
#define SCALA_LEXER_TABLES_H
#include <list>
#include <optional>
#include <unordered_map>

#include "tables.hpp"
#include "semantic/scopes/Scope.h"
#include "semantic/tools/datatype.h"
#include "semantic/tools/tools.h"

class Scope;
class FunDefNode;
class DclNode;
class VarDefsNode;
class ClassDefNode;
class BlockStatsNode;
class ExprNode;
using namespace std;

class MetaInfo;
class BytesMetaInfo;
class Variable;
class FieldMetaInfo;
class LocalVarMetaInfo;
class MethodMetaInfo;
class ClassMetaInfo;
class SemanticContext;

class MetaInfo {
public:
    virtual ~MetaInfo();
};

class BytesMetaInfo : public MetaInfo {
public:
    BytesMetaInfo() = default;

    virtual bytearray_t toBytes() {
        return bytearray_t(); // для кодгена, мб потом параметром ещё и контекст какой-то поступать будет
    }
};

class JvmDescriptorOwner {
public:
    virtual ~JvmDescriptorOwner() = default;

    virtual std::string jvmDescriptor() = 0;
};

class VarMetaInfo : public BytesMetaInfo {
public:
    string name;
    DataType dataType;
    ExprNode *value;
    bool isInit;
    bool isVal;

    VarMetaInfo(string name, const DataType &type, ExprNode *value, bool val)
        : name(name),
          dataType(type),
          value(value),
          isVal(val),
          isInit(true) {
    };

    VarMetaInfo() : isVal(true) {
    }

    std::string toString();

    void setDataType(DataType type);

    bool operator==(const VarMetaInfo &);
};

class FieldMetaInfo : public VarMetaInfo {
public:
    Modifiers modifiers;
    ClassMetaInfo *classMetaInfo = nullptr;

    FieldMetaInfo() : modifiers({}) {
    };
};

class MethodVarMetaInfo : public VarMetaInfo {
public:
    MethodMetaInfo *methodMetaInfo = nullptr;
    uint16_t number = 0;

    MethodVarMetaInfo() = default;
};

class LocalVarMetaInfo : public MethodVarMetaInfo {
public:
    /**
     * Блок к которому принадлежит переменная, нужен для name resolving в методах.
     */
    Scope *scope = nullptr;

    LocalVarMetaInfo();
};

class ArgMetaInfo : public MethodVarMetaInfo {
public:
    ArgMetaInfo() : MethodVarMetaInfo() {
    };
};

class ConstantMetaInfo : public BytesMetaInfo {
    uint16_t number = 0;

    virtual bool operator==(const ConstantMetaInfo &m) const = 0;

    virtual string toString() = 0;
};

class MethodMetaInfo : public BytesMetaInfo {
public:
    string name;
    ClassMetaInfo *classMetaInfo = nullptr;
    Modifiers modifiers;
    DataType returnType;
    ExprNode *body = nullptr;
    // храним в векторе чтобы сохранять очередность аргументов и верно получать кортеж их типов
    vector<ArgMetaInfo *> args;
    /**
     * ключ имя переменной, значение - мапа, где ключ scopeId, а значение - локальная переменная*/
    unordered_map<string, unordered_map<unsigned int, LocalVarMetaInfo *> > localVars;


    uint16_t localVarCounter = 0; // для jvm байт кода

    // возвращает переменную метода
    optional<MethodVarMetaInfo *> resolveLocal(string varName, Scope *scope);

    vector<DataType *> getArgsTypes();

    MethodMetaInfo() : modifiers({}) {
    } ;
};

class ClassMetaInfo : public BytesMetaInfo, public JvmDescriptorOwner {
public:
    string name;
    Modifiers modifiers;
    ClassDefNode *classNode = nullptr;
    ClassMetaInfo *parent = nullptr; // заполняется позже, когда все классы будут занесены в контекст

    unordered_map<string, FieldMetaInfo *> fields;
    /**
     * ключ - имя метода, значение - все сигнатуры метода, чтобы найти искомый, нужно сранивать по типу параметров
     */
    unordered_map<string, vector<MethodMetaInfo *> > methods;
    unordered_map<uint16_t, ConstantMetaInfo *> constants;

    // -----------------------------------------------------------------------------------------------------------------

    ClassMetaInfo(string name, Modifiers modifiers, ClassDefNode *classNode) : name(name),
                                                                               classNode(classNode),
                                                                               modifiers(modifiers) {
    };

    // возвращают соотвественный объект если подобного нет
    // по 2 перегрузки, тк добавляем не только определения, но и декларации(к-ые затем будем анализировать)
    virtual optional<FieldMetaInfo *> addField(VarDefsNode *varDefNode, Modifiers modifiers);

    virtual optional<FieldMetaInfo *> addField(DclNode *varDclNode);

    virtual optional<MethodMetaInfo *> addMethod(FunDefNode *funDefNode, Modifiers modifiers);

    virtual optional<MethodMetaInfo *> addMethod(DclNode *funDclNode);

    // -----------------------------------------------------------------------------------------------------------------

    virtual optional<FieldMetaInfo *> resolveField(string fieldName);

    virtual optional<MethodMetaInfo *> resolveMethod(string methodName, vector<DataType *> argTypes);

    string jvmDescriptor() {
        return "Lvstu/scala/code" + this->name + ";";
    }

protected:
    uint16_t constantCounter = 1;
};

#endif //SCALA_LEXER_TABLES_H
