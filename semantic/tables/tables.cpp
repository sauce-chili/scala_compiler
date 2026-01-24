//
// Created by Dmitry Smirnov on 17.01.2026.
//

#include "tables.hpp"

#include "nodes/class/ClassDefNode.h"
#include "nodes/definitions/DclNode.h"
#include "nodes/func/FunDefNode.h"
#include "nodes/var/VarDefsNode.h"
#include "semantic/SemanticContext.h"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"

class DefNode;
class DclNode;
class FunDefNode;
class VarDefsNode;

MetaInfo::~MetaInfo() = default;

optional<MethodVarMetaInfo *> MethodMetaInfo::resolveLocal(string varName, Scope *scope) {
    // ищем в локальных с учетом скоупа
    auto nameIt = localVars.find(varName);
    if (nameIt != localVars.end()) {
        auto &byScope = nameIt->second;

        // s == nullptr означает, что вышли за пределы метода
        for (Scope *s = scope; s != nullptr; s = s->external) {
            auto scopeIt = byScope.find(s->scopeId);
            if (scopeIt != byScope.end()) {
                return optional{scopeIt->second};
            }
        }
    }

    // ищем в аргументах методов
    for (ArgMetaInfo *arg: args) {
        if (arg->name == varName) {
            return optional{arg};
        }
    }

    // мб дальше в родителя идти
    // не найдено
    return nullopt;
}

vector<DataType *> MethodMetaInfo::getArgsTypes() {
    vector<DataType *> argsTypes;
    for (auto &arg: args) {
        argsTypes.push_back(&arg->dataType);
    }
    return argsTypes;
}

bool ClassMetaInfo::amSubclassOf(const ClassMetaInfo* other) const {
    const ClassMetaInfo* curr = this;
    while (curr) {
        if (curr == other) return true;
        curr = curr->parent;
    }
    return false;
}

optional<FieldMetaInfo *> ClassMetaInfo::resolveField(const string& fieldName,
                                                      const ClassMetaInfo* accessContext,
                                                      bool lookupPrivate) {
    // 1. Ищем поле в текущем классе
    if (fields.find(fieldName) != fields.end()) {
        FieldMetaInfo* field = fields.at(fieldName);

        // Если включен режим принудительного поиска (lookupPrivate = true), отдаем сразу
        if (lookupPrivate) return optional{field};

        // Public: видят все
        bool isPub = !field->isPrivate() && !field->isProtected(); // тк в скале нет публик и все по умолчанию публик
        if (isPub) {
            return optional{field};
        }

        // Private: видит только сам класс
        if (field->isPrivate()) {
            if (accessContext == this) return optional{field};
            return nullopt;
        }

        // Protected: видит сам класс и наследники
        if (field->isProtected()) {
            // Если контекст определен и является наследником текущего класса (или им самим)
            if (accessContext && accessContext->amSubclassOf(this)) {
                return optional{field};
            }
            return nullopt;
        }
    }

    // Если не нашли или не подошло, ищем у родителя
    // При рекурсивном вызове передаем тот же accessContext (кто изначально спрашивал)
    if (parent) {
        return parent->resolveField(fieldName, accessContext, lookupPrivate);
    }

    return nullopt;
}

optional<MethodMetaInfo *> ClassMetaInfo::resolveMethod(const string& methodName,
                                                        const vector<DataType *>& argTypes,
                                                        const ClassMetaInfo* accessFrom,
                                                        bool lookupPrivate) {
    // Ищем метод в текущем классе
    if (methods.find(methodName) != methods.end()) {
        vector<MethodMetaInfo *> methodsSignature = methods.at(methodName);

        for (auto const m: methodsSignature) {
            // Проверка сигнатуры
            vector<DataType *> existingArgs = m->getArgsTypes();
            bool countArgsEquals = existingArgs.size() == argTypes.size();
            bool typesArgsEquals = std::equal(existingArgs.begin(), existingArgs.end(),
                                              argTypes.begin(), argTypes.end(),
                                              [](const DataType *a, const DataType *b) {
                                                  return *a == *b;
                                              });

            if (countArgsEquals && typesArgsEquals) {
                // Метод найден, проверяем доступ
                if (lookupPrivate) return optional{m};

                // Public
                if (!m->isPrivate() && !m->isProtected()) return optional{m};

                // Private
                if (m->isPrivate()) {
                    if (accessFrom == this) return optional{m};
                    continue; // Ищем дальше (перегрузка) или выходим
                }

                // Protected
                if (m->isProtected()) {
                    if (accessFrom && accessFrom->amSubclassOf(this)) return optional{m};
                    continue;
                }
            }
        }
    }

    // Ищем у родителя
    if (parent) {
        return parent->resolveMethod(methodName, argTypes, accessFrom, lookupPrivate);
    }

    return nullopt;
}

optional<string> ClassMetaInfo::getParentName() {
    if (parent) return parent->name;

    if (!classNode || !classNode->classTemplateOpt) return nullopt;
    auto* ext = classNode->classTemplateOpt->extensionPartClassTemplate;
    if (ext && ext->fullId) {
        return optional{ext->fullId->name};
    }
    return nullopt;
}

uint16_t ClassMetaInfo::getConstantCounter() {
    return constantCounter;
}

optional<FieldMetaInfo *> ClassMetaInfo::addField(VarDefsNode *varDefsNode, Modifiers modifiers) {
    if (!varDefsNode || !varDefsNode->fullId) return std::nullopt;

    DataType type = DataType::createFromNode(varDefsNode->simpleType);
    bool isVal = (varDefsNode->type == _VAL_DECL);

    string fieldName = varDefsNode->fullId->name;

    if (fields.count(fieldName)) {
        ErrorTable::addErrorToList(new SemanticError(SemanticError::FieldRedefinition(0, fieldName)));
    } else {

        auto *field = new FieldMetaInfo();
        field->name = fieldName;
        field->jvmName = NameTransformer::encode(name);
        field->dataType = type;
        field->isVal = isVal;
        field->isInit = (varDefsNode->expr != nullptr);
        field->classMetaInfo = this;
        field->modifiers = modifiers;
        field->value = varDefsNode->expr; // Expression stored for later analysis

        this->fields[fieldName] = field;
    }

    return std::nullopt;
}

optional<FieldMetaInfo *> ClassMetaInfo::addField(DclNode *varDclNode) {
    if (!varDclNode || !varDclNode->fullId) return std::nullopt;

    DataType type = DataType::createFromNode(varDclNode->simpleType);
    bool isVal = (varDclNode->type == _VAL_DECL);
    Modifiers mods = Modifiers::createFromModifiersNode(*varDclNode->modifiers);

    string className = varDclNode->fullId->name;

    if (fields.count(className)) {
        ErrorTable::addErrorToList(new SemanticError(SemanticError::FieldRedefinition(0, className)));
    } else {
        auto *field = new FieldMetaInfo();
        field->name = className;
        field->jvmName = NameTransformer::encode(name);
        field->dataType = type;
        field->isVal = isVal;
        field->classMetaInfo = this;
        field->modifiers = mods;
        field->isInit = false;

        this->fields[className] = field;
    }

    return std::nullopt;
}

optional<MethodMetaInfo *> ClassMetaInfo::addMethod(FunDefNode *funDefNode, Modifiers modifiers) {
    if (!funDefNode || !funDefNode->funSig || !funDefNode->funSig->fullId) return std::nullopt;

    string methodName = funDefNode->funSig->fullId->name;

    // Сбор временных аргументов для получения их типов
    vector<ArgMetaInfo *> tempArgs;
    vector<DataType *> signatureTypes; // Указатели на типы для проверки

    int tempCounter = 0;
    if (funDefNode->funSig->params && funDefNode->funSig->params->funcParams) {
        for (auto *paramNode: *funDefNode->funSig->params->funcParams) {
            ArgMetaInfo *arg = new ArgMetaInfo();
            arg->name = paramNode->fullId->name;
            arg->jvmName = NameTransformer::encode(name);
            arg->dataType = DataType::createFromNode(paramNode->simpleType);
            arg->number = tempCounter++;
            arg->isVal = true; // Аргументы в Scala неизменяемы (val)

            tempArgs.push_back(arg);
            // Сохраняем указатель на тип для передачи в resolveMethod
            signatureTypes.push_back(&tempArgs.back()->dataType);
        }
    }

    // Проверка дубликатов через resolveMethod
    if (this->resolveMethod(methodName, signatureTypes, this).has_value()) {
        string sigStr = methodName + "(";
        for (size_t i = 0; i < signatureTypes.size(); ++i) {
            sigStr += signatureTypes[i]->toString();
            if (i < signatureTypes.size() - 1) sigStr += ", ";
        }
        sigStr += ")";

        ErrorTable::addErrorToList(new SemanticError(SemanticError::MethodAlreadyExists(0, sigStr)));

        // Очищаем память временных аргументов, так как метод не создан
        for (auto *a: tempArgs) delete a;
        return std::nullopt;
    }

    // Метод уникален, создаем MetaInfo
    MethodMetaInfo *method = new MethodMetaInfo();
    method->name = methodName;
    method->jvmName = NameTransformer::encode(methodName);
    method->classMetaInfo = this;
    method->body = funDefNode->expr;
    method->returnType = DataType::createFromNode(funDefNode->simpleType);
    method->modifiers = modifiers;
    method->localVarCounter = tempCounter;

    // Переносим аргументы в структуру метода
    for (auto *arg: tempArgs) {
        arg->methodMetaInfo = method;
        method->args.push_back(arg);
    }

    this->methods[methodName].push_back(method);
    return method;
}

optional<MethodMetaInfo *> ClassMetaInfo::addMethod(DclNode *funDclNode) {
    if (!funDclNode || !funDclNode->funSig || !funDclNode->funSig->fullId) return std::nullopt;

    string methodName = funDclNode->funSig->fullId->name;

    // Сбор типов аргументов (аналогично addMethod для Def)
    vector<ArgMetaInfo *> tempArgs;
    vector<DataType *> signatureTypes;
    int tempCounter = 0;

    if (funDclNode->funSig->params && funDclNode->funSig->params->funcParams) {
        for (auto *paramNode: *funDclNode->funSig->params->funcParams) {
            ArgMetaInfo *arg = new ArgMetaInfo();
            arg->name = paramNode->fullId->name;
            arg->jvmName = NameTransformer::encode(name);
            arg->dataType = DataType::createFromNode(paramNode->simpleType);
            arg->number = tempCounter++;
            arg->isVal = true;

            tempArgs.push_back(arg);
            signatureTypes.push_back(&tempArgs.back()->dataType);
        }
    }

    // Проверка дубликатов
    if (this->resolveMethod(methodName, signatureTypes, this).has_value()) {
        string sigStr = methodName + "(...)";
        ErrorTable::addErrorToList(
            new SemanticError(SemanticError::MethodAlreadyExists(funDclNode->funSig->fullId->id, sigStr)));

        for (auto *a: tempArgs) delete a;
        return std::nullopt;
    }

    // Регистрация метода
    MethodMetaInfo *method = new MethodMetaInfo();
    method->name = methodName;
    method->jvmName = NameTransformer::encode(methodName);
    method->classMetaInfo = this;
    method->returnType = DataType::createFromNode(funDclNode->simpleType);
    method->modifiers = Modifiers::createFromModifiersNode(*funDclNode->modifiers);
    method->localVarCounter = tempCounter;

    for (auto *arg: tempArgs) {
        arg->methodMetaInfo = method;
        method->args.push_back(arg);
    }

    this->methods[methodName].push_back(method);
    return method;
}

RtlClassMetaInfo* RtlClassMetaInfo::Object = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Any = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::String = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Integer = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::SbtIn = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Unit = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Char = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Double = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Boolean = nullptr;

static std::unordered_map<std::string, RtlClassMetaInfo*> rtlClassMap;

void RtlClassMetaInfo::initializeRtlClasses() {
    Object = RtlClassMetaInfo::initObject();
    String = RtlClassMetaInfo::initString();
    Any = RtlClassMetaInfo::initAny();
    Integer = RtlClassMetaInfo::initInteger();
    Double = RtlClassMetaInfo::initDouble();
    Boolean = RtlClassMetaInfo::initBoolean();
    Char = RtlClassMetaInfo::initChar();
    Unit = RtlClassMetaInfo::initUnit();
    SbtIn = RtlClassMetaInfo::initSbtIn();

    rtlClassMap["String"] = String;
    rtlClassMap["Any"] = Any;
    rtlClassMap["Int"] = Integer;
    rtlClassMap["Double"] = Double;
    rtlClassMap["Boolean"] = Boolean;
    rtlClassMap["Char"] = Char;
    rtlClassMap["Unit"] = Unit;
    rtlClassMap["SbtIn"] = SbtIn;

    std::cout << "RTL classes initialized successfully." << std::endl;
}

RtlClassMetaInfo* RtlClassMetaInfo::getRtlClassInfo(const std::string& typeName) {
    auto it = rtlClassMap.find(typeName);
    if (it != rtlClassMap.end()) {
        return it->second;
    }
    return nullptr;
}

RtlClassMetaInfo *RtlClassMetaInfo::initAny() {
    string className = "Any";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _SCALA);

    rec->parent = Object;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_ABSTRACT);

    MethodMetaInfo* toScalaString = new MethodMetaInfo();
    toScalaString->classMetaInfo = Any;
    toScalaString->modifiers.modifiers.push_back(_PUBLIC);
    toScalaString->returnType = DataType::Kind::Class;
    toScalaString->returnType.className = "String";
    toScalaString->name = "toScalaString";
    toScalaString->jvmName = NameTransformer::encode(toScalaString->name);
    toScalaString->args = vector<ArgMetaInfo*>();
    rec->methods[toScalaString->name].push_back(toScalaString);

    MethodMetaInfo* isInstanceOf = new MethodMetaInfo();
    isInstanceOf->classMetaInfo = Any;
    isInstanceOf->modifiers.modifiers.push_back(_PUBLIC);
    isInstanceOf->returnType = DataType::Kind::Class;
    isInstanceOf->returnType.className = "Boolean";
    isInstanceOf->name = "isInstanceOf";
    isInstanceOf->jvmName = NameTransformer::encode(isInstanceOf->name);
    isInstanceOf->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* isInstanceOfArg1= new ArgMetaInfo();
    isInstanceOfArg1->name = "cls";
    isInstanceOfArg1->jvmName = NameTransformer::encode(isInstanceOfArg1->name);
    isInstanceOfArg1->dataType = DataType::Kind::Class;
    isInstanceOfArg1->dataType.className = "Object";
    isInstanceOf->args.push_back(isInstanceOfArg1);
    rec->methods[isInstanceOf->name].push_back(isInstanceOf);

    return rec;
}

RtlClassMetaInfo* RtlClassMetaInfo::initString() {
    return new RtlClassMetaInfo("String", _JAVA);
}

RtlClassMetaInfo* RtlClassMetaInfo::initInteger() {
    return new RtlClassMetaInfo("Int", _JAVA);
}

RtlClassMetaInfo* RtlClassMetaInfo::initSbtIn() {
    return new RtlClassMetaInfo("SbtIn", _JAVA);
}

RtlClassMetaInfo* RtlClassMetaInfo::initUnit() {
    return new RtlClassMetaInfo("Unit", _SCALA);
}

RtlClassMetaInfo* RtlClassMetaInfo::initChar() {
    return new RtlClassMetaInfo("Char", _JAVA);
}

RtlClassMetaInfo* RtlClassMetaInfo::initDouble() {
    return new RtlClassMetaInfo("Double", _JAVA);
}

RtlClassMetaInfo* RtlClassMetaInfo::initBoolean() {
    string className = "Boolean";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _JAVA);

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = Boolean;
    constructor->modifiers.modifiers.push_back(_PRIVATE);
    constructor->returnType = DataType::Kind::Class;
    constructor->returnType.className = "Object";
    constructor->name = "<init>";
    constructor->jvmName = NameTransformer::encode(constructor->name);
    constructor->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* constructor1= new ArgMetaInfo();
    constructor1->name = "v";
    constructor1->jvmName = NameTransformer::encode(constructor1->name);
    constructor1->dataType = DataType::Kind::Bool;
    constructor->args.push_back(constructor1);
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* booleanValue = new MethodMetaInfo();
    booleanValue->classMetaInfo = Boolean;
    booleanValue->modifiers.modifiers.push_back(_PUBLIC);
    booleanValue->returnType = DataType::Kind::Bool;
    booleanValue->name = "booleanValue";
    booleanValue->jvmName = NameTransformer::encode(booleanValue->name);
    booleanValue->args = vector<ArgMetaInfo*>();
    rec->methods[booleanValue->name].push_back(booleanValue);

    MethodMetaInfo* andM = new MethodMetaInfo();
    andM->classMetaInfo = Boolean;
    andM->modifiers.modifiers.push_back(_PUBLIC);
    andM->returnType = DataType::Kind::Class;
    andM->returnType.className = "Boolean";
    andM->name = "and";
    andM->jvmName = NameTransformer::encode(andM->name);
    andM->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* andM1= new ArgMetaInfo();
    andM1->name = "other";
    andM1->jvmName = NameTransformer::encode(andM1->name);
    andM1->dataType = DataType::Kind::Bool;
    andM1->dataType.className = "Boolean";
    andM->args.push_back(andM1);
    rec->methods[andM->name].push_back(andM);

    MethodMetaInfo* orM = new MethodMetaInfo();
    orM->classMetaInfo = Boolean;
    orM->modifiers.modifiers.push_back(_PUBLIC);
    orM->returnType = DataType::Kind::Class;
    orM->returnType.className = "Boolean";
    orM->name = "or";
    orM->jvmName = NameTransformer::encode(orM->name);
    orM->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* orM1= new ArgMetaInfo();
    orM1->name = "other";
    orM1->jvmName = NameTransformer::encode(orM1->name);
    orM1->dataType = DataType::Kind::Bool;
    orM1->dataType.className = "Boolean";
    orM->args.push_back(orM1);
    rec->methods[orM->name].push_back(orM);

    MethodMetaInfo* xorM = new MethodMetaInfo();
    xorM->classMetaInfo = Boolean;
    xorM->modifiers.modifiers.push_back(_PUBLIC);
    xorM->returnType = DataType::Kind::Class;
    xorM->returnType.className = "Boolean";
    xorM->name = "xor";
    xorM->jvmName = NameTransformer::encode(xorM->name);
    xorM->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* xorM1= new ArgMetaInfo();
    xorM1->name = "other";
    xorM1->jvmName = NameTransformer::encode(xorM1->name);
    xorM1->dataType = DataType::Kind::Bool;
    xorM1->dataType.className = "Boolean";
    xorM->args.push_back(xorM1);
    rec->methods[xorM->name].push_back(xorM);

    MethodMetaInfo* notM = new MethodMetaInfo();
    notM->classMetaInfo = Boolean;
    notM->modifiers.modifiers.push_back(_PUBLIC);
    notM->returnType = DataType::Kind::Class;
    notM->returnType.className = "Boolean";
    notM->name = "not";
    notM->jvmName = NameTransformer::encode(notM->name);
    notM->args = vector<ArgMetaInfo*>();
    rec->methods[notM->name].push_back(notM);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = Boolean;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg1= new ArgMetaInfo();
    equalsArg1->name = "o";
    equalsArg1->jvmName = NameTransformer::encode(equalsArg1->name);
    equalsArg1->dataType = DataType::Kind::Class;
    equalsArg1->dataType.className = "Object";
    equals->args.push_back(equalsArg1);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = Boolean;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = Boolean;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->modifiers.modifiers.push_back(_OVERRIDE);
    toString->returnType = DataType::Kind::Class;
    toString->returnType.className = "String";
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    return rec;
}

RtlClassMetaInfo *RtlClassMetaInfo::initObject() {
    string className = "Object";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _SCALA);

    rec->parent = nullptr;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_ABSTRACT);

    MethodMetaInfo* toJvmString = new MethodMetaInfo();
    toJvmString->classMetaInfo = Object;
    toJvmString->modifiers.modifiers.push_back(_PUBLIC);
    toJvmString->returnType = DataType::Kind::Class;
    toJvmString->returnType.className = "String";
    toJvmString->name = "toJvmString";
    toJvmString->jvmName = NameTransformer::encode(toJvmString->name);
    toJvmString->args = vector<ArgMetaInfo*>();
    rec->methods[toJvmString->name].push_back(toJvmString);

    MethodMetaInfo* ToString = new MethodMetaInfo(); // Оно с заглавной в коде начинается
    ToString->classMetaInfo = Object;
    ToString->modifiers.modifiers.push_back(_PUBLIC);
    ToString->returnType = DataType::Kind::Class;
    ToString->returnType.className = "String";
    ToString->name = "ToString";
    ToString->jvmName = NameTransformer::encode(ToString->name);
    ToString->args = vector<ArgMetaInfo*>();
    rec->methods[ToString->name].push_back(ToString);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = Object;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->returnType = DataType::Kind::Class;
    toString->returnType.className = "String";
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = Object;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_ABSTRACT);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg1= new ArgMetaInfo();
    equalsArg1->name = "other";
    equalsArg1->jvmName = NameTransformer::encode(equalsArg1->name);
    equalsArg1->dataType = DataType::Kind::Class;
    equalsArg1->dataType.className = "Object";
    equals->args.push_back(equalsArg1);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = Object;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_ABSTRACT);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* notEquals = new MethodMetaInfo();
    notEquals->classMetaInfo = Object;
    notEquals->modifiers.modifiers.push_back(_PUBLIC);
    notEquals->returnType = DataType::Kind::Class;
    notEquals->returnType.className = "Boolean";
    notEquals->name = "notEquals";
    notEquals->jvmName = NameTransformer::encode(notEquals->name);
    notEquals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* notEqualsArg1= new ArgMetaInfo();
    notEqualsArg1->name = "other";
    notEqualsArg1->jvmName = NameTransformer::encode(notEqualsArg1->name);
    notEqualsArg1->dataType = DataType::Kind::Class;
    notEqualsArg1->dataType.className = "Object";
    notEquals->args.push_back(notEqualsArg1);
    rec->methods[notEquals->name].push_back(notEquals);

    MethodMetaInfo* is = new MethodMetaInfo();
    is->classMetaInfo = Object;
    is->modifiers.modifiers.push_back(_PUBLIC);
    is->returnType = DataType::Kind::Class;
    is->returnType.className = "Boolean";
    is->name = "is";
    is->jvmName = NameTransformer::encode(is->name);
    is->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* isArg1= new ArgMetaInfo();
    isArg1->name = "other";
    isArg1->jvmName = NameTransformer::encode(isArg1->name);
    isArg1->dataType = DataType::Kind::Class;
    isArg1->dataType.className = "Object";
    is->args.push_back(isArg1);
    rec->methods[is->name].push_back(is);

    MethodMetaInfo* isNot = new MethodMetaInfo();
    isNot->classMetaInfo = Object;
    isNot->modifiers.modifiers.push_back(_PUBLIC);
    isNot->returnType = DataType::Kind::Class;
    isNot->returnType.className = "Boolean";
    isNot->name = "isNot";
    isNot->jvmName = NameTransformer::encode(isNot->name);
    isNot->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* isNotArg1= new ArgMetaInfo();
    isNotArg1->name = "other";
    isNotArg1->jvmName = NameTransformer::encode(isNotArg1->name);
    isNotArg1->dataType = DataType::Kind::Class;
    isNotArg1->dataType.className = "Object";
    isNot->args.push_back(isNotArg1);
    rec->methods[isNot->name].push_back(isNot);

    return rec;
}
