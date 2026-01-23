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

    bool isPrivate() const { return modifiers.hasModifier(_PRIVATE); }
    bool isProtected() const { return modifiers.hasModifier(_PROTECTED); }
    bool isFinal() const { return modifiers.hasModifier(_FINAL); }
    bool isOverride() const { return modifiers.hasModifier(_OVERRIDE); }
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

    bool isPrivate() const { return modifiers.hasModifier(_PRIVATE); }
    bool isProtected() const { return modifiers.hasModifier(_PROTECTED); }
    bool isFinal() const { return modifiers.hasModifier(_FINAL); }
    bool isOverride() const { return modifiers.hasModifier(_OVERRIDE); }
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
                                                                               modifiers(modifiers),
                                                                               classNode(classNode) {
    };
    ClassMetaInfo(string name, Modifiers modifiers) : name(name), modifiers(modifiers) {
    };

    // возвращают соотвественный объект если подобного нет
    // по 2 перегрузки, тк добавляем не только определения, но и декларации(к-ые затем будем анализировать)
    virtual optional<FieldMetaInfo *> addField(VarDefsNode *varDefNode, Modifiers modifiers);

    virtual optional<FieldMetaInfo *> addField(DclNode *varDclNode);

    virtual optional<MethodMetaInfo *> addMethod(FunDefNode *funDefNode, Modifiers modifiers);

    virtual optional<MethodMetaInfo *> addMethod(DclNode *funDclNode);

    // -----------------------------------------------------------------------------------------------------------------

    bool amSubclassOf(const ClassMetaInfo* other) const;

    virtual optional<FieldMetaInfo *> resolveField(const string& fieldName,
                                                   const ClassMetaInfo* accessContext,
                                                   bool lookupPrivate = false);

    virtual optional<MethodMetaInfo *> resolveMethod(const string& methodName,
                                                     const vector<DataType *>& argTypes,
                                                     const ClassMetaInfo* accessFrom,
                                                     bool lookupPrivate = false);

    string jvmDescriptor() {
        return "Lvstu/scala/code" + this->name + ";";
    }

    optional<string> getParentName();

    uint16_t getConstantCounter();

    bool isPrivate() const { return modifiers.hasModifier(_PRIVATE); }
    bool isProtected() const { return modifiers.hasModifier(_PROTECTED); }
    bool isFinal() const { return modifiers.hasModifier(_FINAL); }
    bool isOverride() const { return modifiers.hasModifier(_OVERRIDE); }

protected:
    uint16_t constantCounter = 1;
};

class RtlClassMetaInfo : public ClassMetaInfo {
public:
    static RtlClassMetaInfo* String;
    static RtlClassMetaInfo* Integer;
    static RtlClassMetaInfo* SbtIn;
    static RtlClassMetaInfo* Unit;
    static RtlClassMetaInfo* Char;
    static RtlClassMetaInfo* Double;
    static RtlClassMetaInfo* Boolean;

    std::string javaDescriptor = "java/lang/";
    std::string scalaDescriptor = "Lscala/runtime/";

    enum Lang {
        _SCALA,
        _JAVA
    };

    Lang langOfDescriptor;

    string jvmDescriptor() {
        return langOfDescriptor == _SCALA ? scalaDescriptor : javaDescriptor
               + this->name + ";";
    }

    RtlClassMetaInfo(std::string name, Lang langOfDescriptor): ClassMetaInfo(name, Modifiers({})) {
        this->langOfDescriptor = langOfDescriptor;
    }

    static RtlClassMetaInfo* getRtlClassInfo(const std::string& typeName);

public:
    static void initializeRtlClasses();

private:
    static RtlClassMetaInfo* initString();
    static RtlClassMetaInfo* initInteger();
    static RtlClassMetaInfo* initSbtIn();
    static RtlClassMetaInfo* initUnit();
    static RtlClassMetaInfo* initChar();
    static RtlClassMetaInfo* initDouble();
    static RtlClassMetaInfo* initBoolean();
};

#endif //SCALA_LEXER_TABLES_H
