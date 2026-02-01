//
// Created by Dmitry Smirnov on 17.01.2026.
//

#ifndef SCALA_LEXER_TABLES_H
#define SCALA_LEXER_TABLES_H
#include <inttypes.h>
#include <list>
#include <optional>
#include <unordered_map>
#include <string>

#include "tables.hpp"
#include "nodes/generator/GeneratorNode.h"
#include "semantic/scopes/Scope.h"
#include "semantic/tools/datatype.h"
#include "semantic/tools/tools.h"
#include "semantic/tools/NameTransformer.h"

// Имя конструктора в Scala (после преобразования AST)
extern const std::string CONSTRUCTOR_NAME;
// Имя конструктора в JVM байткоде
extern const std::string JVM_CONSTRUCTOR_NAME;

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
    string jvmName;
    DataType dataType;
    ExprNode *value;
    bool isInit;
    bool isVal;

    VarMetaInfo(string name, const DataType &type, ExprNode *value, bool val)
        : name(name),
          jvmName(NameTransformer::encode(name)),
          dataType(type),
          value(value),
          isVal(val),
          isInit(false) {
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

class MethodMetaInfo : public BytesMetaInfo, public JvmDescriptorOwner {
public:
    string name;
    string jvmName;
    ClassMetaInfo *classMetaInfo = nullptr;
    Modifiers modifiers;
    DataType returnType;
    ExprNode *body = nullptr;
    bool isPrimaryConstructor = false;
    // храним в векторе чтобы сохранять очередность аргументов и верно получать кортеж их типов
    vector<ArgMetaInfo *> args;
    /**
     * ключ имя переменной, значение - мапа, где ключ scopeId, а значение - локальная переменная
     */
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

    /**
     * Возвращает JVM-дескриптор метода.
     * Формат: (параметры)возвращаемый_тип
     * Пример: (II)I для метода (Int, Int) => Int
     */
    std::string jvmDescriptor() override;

    virtual optional<LocalVarMetaInfo *> addLocalVar(VarDefsNode *varDefsNode, Scope *scope);
    virtual optional<LocalVarMetaInfo *> addGeneratorVar(GeneratorNode *generatorNode, Scope *scope);
    virtual optional<LocalVarMetaInfo *> executeAssign(AssignmentNode *assignNode, Scope* scope);
};

class ClassMetaInfo : public BytesMetaInfo, public JvmDescriptorOwner {
public:
    string name;
    string jvmName;
    Modifiers modifiers;
    ClassDefNode *classNode = nullptr;
    ClassMetaInfo *parent = nullptr; // заполняется позже, когда все классы будут занесены в контекст

    unordered_map<string, FieldMetaInfo *> fields;
    /**
     * ключ - имя метода, значение - все сигнатуры метода, чтобы найти искомый, нужно сранивать по типу параметров
     */
    unordered_map<string, vector<MethodMetaInfo *> > methods;
    class ConstantPoolBuilder* constantPool = nullptr;  // Таблица констант для кодогенерации

    // -----------------------------------------------------------------------------------------------------------------

    ClassMetaInfo(string name, Modifiers modifiers, ClassDefNode *classNode) : name(name),
                                                                               jvmName(NameTransformer::encode(name)),
                                                                               modifiers(modifiers),
                                                                               classNode(classNode) {
    };
    ClassMetaInfo(string name, Modifiers modifiers) :
                                                      name(name),
                                                      jvmName(NameTransformer::encode(name)),
                                                      modifiers(modifiers) {
    };

    // возвращают соотвественный объект если подобного нет
    // по 2 перегрузки, тк добавляем не только определения, но и декларации(к-ые затем будем анализировать)
    virtual optional<FieldMetaInfo *> addField(VarDefsNode *varDefNode, Modifiers modifiers);

    virtual optional<FieldMetaInfo *> addField(DclNode *varDclNode);

    virtual optional<MethodMetaInfo *> addMethod(FunDefNode *funDefNode, Modifiers modifiers);

    virtual optional<MethodMetaInfo *> addMethod(DclNode *funDclNode);

    // -----------------------------------------------------------------------------------------------------------------

    /**
     * Проверяет, является ли текущий класс подклассом (наследником) указанного класса.
     *
     * Метод проходит вверх по иерархии наследования (через указатели `parent`),
     * чтобы выяснить, присутствует ли `other` среди предков текущего класса.
     *
     * Особенности:
     * - Реализует рефлексивность: класс считается подклассом самого себя (возвращает true, если `this == other`).
     *
     * @param other Указатель на мета-информацию потенциального родительского класса.
     * @return true Если текущий класс наследуется от `other` или является им.
     * @return false Если конец иерархии достигнут, а совпадение не найдено.
 */
    bool amSubclassOf(const ClassMetaInfo *other) const;

    /**
     * Ищет поле с указанным именем в текущем классе и вверх по иерархии наследования.
     * Метод учитывает модификаторы доступа поля и контекст, из которого происходит обращение.
     *
     * Алгоритм поиска:
     * 1. Ищет поле в текущем классе.
     * 2. Если поле найдено:
     *  - Если `lookupPrivate == true`, возвращает поле игнорируя права доступа.
     *  - Если поле Public: возвращает поле.
     *  - Если поле Private: возвращает поле только если `accessFrom` совпадает с текущим классом.
     *  - Если поле Protected: возвращает поле, если `accessFrom` является наследником текущего класса.
     * 3. Если поле не найдено или недоступно, поиск рекурсивно продолжается в родительском классе (`parent`).
     *
     * @param fieldName     Имя искомого поля.
     * @param accessFrom    Мета-информация о классе, который запрашивает доступ (контекст вызова).
     * Может быть `nullptr`, если контекст неизвестен.
     * @param lookupPrivate Флаг принудительного поиска. Если `true`, игнорирует проверки видимости
     * (полезно для отладки или системных вызовов).
     * @return std::optional<FieldMetaInfo*> Указатель на мета-информацию о поле, если оно найдено и доступно,
     * иначе `std::nullopt`.
     */
    virtual optional<FieldMetaInfo *> resolveField(
        const string &fieldName,
        const ClassMetaInfo *accessFrom,
        bool lookupPrivate = false
    );

    /**
     * Ищет метод по имени и сигнатуре аргументов в текущем классе и иерархии наследования.
     * Поддерживает перегрузку методов (overloading) и проверку прав доступа.
     *
     * Алгоритм поиска:
     * 1. Получает список всех методов с именем `methodName` в текущем классе.
     * 2. Перебирает кандидатов, проверяя точное совпадение количества и типов аргументов (`argTypes`).
     * 3. Если подходящий кандидат найден:
     *  - Проверяет права доступа (Public/Private/Protected) аналогично логике полей.
     *  - Если метод приватный и контекст не позволяет доступ, поиск продолжается (на случай перегрузок) или завершается.
     * 4. Если подходящий метод не найден в текущем классе, поиск рекурсивно уходит к родителю (`parent`).
     *
     * @param methodName    Имя искомого метода.
     * @param argTypes      Список типов аргументов для разрешения перегрузки (Signature matching).
     * @param accessFrom    Мета-информация о классе, запрашивающем доступ. Необходим для проверки
     * `protected` и `private` методов.
     * @param lookupPrivate Флаг принудительного поиска, отключающий проверки видимости.
     * @return std::optional<MethodMetaInfo *> Указатель на мета-информацию о методе, если он найден,
     * соответствует сигнатуре и доступен. Иначе `std::nullopt`.
     */
    virtual optional<MethodMetaInfo *> resolveMethod(
        const string &methodName,
        const vector<DataType *> &argTypes,
        const ClassMetaInfo *accessFrom,
        bool lookupPrivate = false
    );

    string jvmDescriptor() {
        return "Lvstu/scala/code" + this->name + ";";
    }

    optional<string> getParentName();

    uint16_t getConstantCounter();

    bool isPrivate() const { return modifiers.hasModifier(_PRIVATE); }
    bool isProtected() const { return modifiers.hasModifier(_PROTECTED); }
    bool isFinal() const { return modifiers.hasModifier(_FINAL); }
    bool isOverride() const { return modifiers.hasModifier(_OVERRIDE); }

    /**
     * Проверяет, является ли класс RTL классом (встроенным в runtime)
     */
    bool isRTL() const;

};

class RtlClassMetaInfo : public ClassMetaInfo {
public:
    static RtlClassMetaInfo* Any;
    static RtlClassMetaInfo* Object;
    static RtlClassMetaInfo* String;
    static RtlClassMetaInfo* Integer;
    static RtlClassMetaInfo* StdIn;
    static RtlClassMetaInfo* Predef;
    static RtlClassMetaInfo* Unit;
    static RtlClassMetaInfo* Char;
    static RtlClassMetaInfo* Double;
    static RtlClassMetaInfo* Boolean;
    static RtlClassMetaInfo* Array;

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
    static RtlClassMetaInfo* initAny();
    static RtlClassMetaInfo* initObject();
    static RtlClassMetaInfo* initString();
    static RtlClassMetaInfo* initInteger();
    static RtlClassMetaInfo* initStdIn();
    static RtlClassMetaInfo* initPredef();
    static RtlClassMetaInfo* initUnit();
    static RtlClassMetaInfo* initChar();
    static RtlClassMetaInfo* initDouble();
    static RtlClassMetaInfo* initBoolean();
    static RtlClassMetaInfo* initArray();
};

#endif //SCALA_LEXER_TABLES_H
