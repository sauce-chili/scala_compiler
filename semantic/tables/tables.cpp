//
// Created by Dmitry Smirnov on 17.01.2026.
//

#include "tables.hpp"

#include "nodes/class/ClassDefNode.h"
#include "semantic/constants/ConstantPoolBuilder.h"
#include "nodes/definitions/DclNode.h"
#include "nodes/func/FunDefNode.h"
#include "nodes/var/VarDefsNode.h"
#include "semantic/SemanticContext.h"
#include "nodes/exprs/AssignmentNode.h"

class DefNode;
class DclNode;
class FunDefNode;
class VarDefsNode;

// Определение констант для имён конструкторов
const std::string CONSTRUCTOR_NAME = "this";
const std::string JVM_CONSTRUCTOR_NAME = "<init>";
const std::string BASE_SCALA_CLASS = "Any";

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

std::string MethodMetaInfo::jvmDescriptor() {
    std::string descriptor = "(";
    for (const auto* arg : args) {
        descriptor += arg->dataType.toJvmDescriptor();
    }
    descriptor += ")";
    descriptor += returnType.toJvmDescriptor();
    return descriptor;
}

optional<LocalVarMetaInfo *> MethodMetaInfo::addLocalVar(VarDefsNode *varDefsNode, Scope* scope) {
    if (!varDefsNode || !varDefsNode->fullId || !scope) return nullopt;

    string varName = varDefsNode->fullId->name;

    // Проверяем: не определена ли уже переменная с таким именем в ТЕКУЩЕМ скоупе
    auto& byName = localVars[varName];
    auto it = byName.find(scope->scopeId);
    if (it != byName.end()) {
        // Переменная уже определена в этом скоупе
        auto* foundVar = it->second;
        // Если она до этого не была инициализирована, инициализируем ее
        if (!foundVar->isInit && varDefsNode->expr) {
            foundVar->isInit = true;
            foundVar->value = varDefsNode->expr;
        }

        return nullopt;
    }

    DataType type = DataType::createFromNode(varDefsNode->simpleType);
    bool isVal = (varDefsNode->type == _VAL_DECL);

    // Создаем LocalVarMetaInfo
    LocalVarMetaInfo* localVar = new LocalVarMetaInfo();
    localVar->name = varName;
    localVar->jvmName = NameTransformer::encode(varName);
    localVar->dataType = type;
    localVar->value = varDefsNode->expr;
    localVar->isVal = isVal;
    localVar->isInit = (varDefsNode->expr != nullptr);
    localVar->scope = scope;
    localVar->methodMetaInfo = this;
    localVar->number = localVarCounter++;

    // Добавляем в таблицу
    localVars[varName][scope->scopeId] = localVar;

    return localVar;
}

optional<LocalVarMetaInfo *> MethodMetaInfo::addGeneratorVar(GeneratorNode *generatorNode, Scope *scope) {
    if (!generatorNode || !generatorNode->fullId || !scope) return nullopt;

    string varName = generatorNode->fullId->name;

    // Проверяем: не определена ли уже переменная с таким именем в ТЕКУЩЕМ скоупе
    auto& byName = localVars[varName];
    auto it = byName.find(scope->scopeId);
    if (byName.find(scope->scopeId) != byName.end()) {
        // Переменная уже определена в этом скоупе
        auto* foundVar = it->second;
        // Если она до этого не была инициализирована, инициализируем ее
        if (!foundVar->isInit && generatorNode->expr) {
            foundVar->isInit = true;
            foundVar->value = generatorNode->expr;
        }

        return nullopt;
    }

    DataType type = DataType::createFromNode(generatorNode->simpleType);

    LocalVarMetaInfo* localVar = new LocalVarMetaInfo();
    localVar->name = varName;
    localVar->jvmName = NameTransformer::encode(varName);
    localVar->dataType = type;
    localVar->value = nullptr; // значение пишется каждый раз
    localVar->isVal = true; // В for-генераторах переменные всегда val
    localVar->isInit = true;
    localVar->scope = scope;
    localVar->methodMetaInfo = this;
    localVar->number = localVarCounter++;

    localVars[varName][scope->scopeId] = localVar;
    return localVar;
}

bool ClassMetaInfo::amSubclassOf(const ClassMetaInfo* other) const {
    const ClassMetaInfo* curr = this;
    while (curr) {
        if (curr == other) return true;
        curr = curr->parent;
    }
    return false;
}

bool ClassMetaInfo::isRTL() const {
    return dynamic_cast<const RtlClassMetaInfo*>(this) != nullptr;
}

optional<LocalVarMetaInfo *> MethodMetaInfo::executeAssign(AssignmentNode *assignNode, Scope* scope) {
    if (!assignNode || !assignNode->expr || !assignNode->fullId || !scope) return nullopt;

    string varName = assignNode->fullId->name;

    // Проверяем: не определена ли уже переменная с таким именем в ТЕКУЩЕМ скоупе
    optional<MethodVarMetaInfo*> optLocalVar = resolveLocal(varName, scope);
    if (optLocalVar.has_value()) {
        MethodVarMetaInfo* localVar = optLocalVar.value();
        localVar->isInit = true;
        localVar->value = assignNode->expr;
    }

    optional<FieldMetaInfo*> optField = classMetaInfo->resolveField(varName, classMetaInfo, false);
    if (optField.has_value()) {
        FieldMetaInfo* foundField = optField.value();
        foundField->isInit = true;
        foundField->value = assignNode->expr;
    }

    return nullopt;
}

optional<FieldMetaInfo *> ClassMetaInfo::resolveField(const string& fieldName,
                                                      const ClassMetaInfo* accessFrom,
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
            if (accessFrom == this) return optional{field};
            return nullopt;
        }

        // Protected: видит сам класс и наследники
        if (field->isProtected()) {
            // Если контекст определен и является наследником текущего класса (или им самим)
            if (accessFrom && accessFrom->amSubclassOf(this)) {
                return optional{field};
            }
            return nullopt;
        }
    }

    // Если не нашли или не подошло, ищем у родителя
    // При рекурсивном вызове передаем тот же accessContext (кто изначально спрашивал)
    if (parent) {
        return parent->resolveField(fieldName, accessFrom, lookupPrivate);
    }

    return nullopt;
}

optional<MethodMetaInfo *> ClassMetaInfo::resolveMethod(const string& methodName,
                                                        const vector<DataType *>& argTypes,
                                                        const ClassMetaInfo* accessFrom,
                                                        int leftParents,
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
    if (parent && leftParents > 0) {
        return parent->resolveMethod(methodName, argTypes, accessFrom, leftParents--, lookupPrivate);
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
    if (constantPool) {
        return constantPool->size();
    }
    return 0;
}

optional<FieldMetaInfo *> ClassMetaInfo::addField(VarDefsNode *varDefsNode, Modifiers modifiers) {
    if (!varDefsNode || !varDefsNode->fullId) return nullopt;

    string fieldName = varDefsNode->fullId->name;

    // Поле уже существует - возвращаем nullopt, вызывающая сторона добавит ошибку
    if (fields.count(fieldName)) {
        return nullopt;
    }

    DataType type = DataType::createFromNode(varDefsNode->simpleType);
    bool isVal = (varDefsNode->type == _VAL_DECL);

    auto *field = new FieldMetaInfo();
    field->name = fieldName;
    field->jvmName = NameTransformer::encode(fieldName);
    field->dataType = type;
    field->isVal = isVal;
    field->isInit = (varDefsNode->expr != nullptr);
    field->classMetaInfo = this;
    field->modifiers = modifiers;
    field->value = varDefsNode->expr;

    this->fields[fieldName] = field;
    return field;
}

optional<FieldMetaInfo *> ClassMetaInfo::addField(DclNode *varDclNode) {
    if (!varDclNode || !varDclNode->fullId) return nullopt;

    string fieldName = varDclNode->fullId->name;

    // Поле уже существует - возвращаем nullopt
    if (fields.count(fieldName)) {
        return nullopt;
    }

    DataType type = DataType::createFromNode(varDclNode->simpleType);
    bool isVal = (varDclNode->type == _VAL_DECL);
    Modifiers mods = Modifiers::createFromModifiersNode(*varDclNode->modifiers);

    auto *field = new FieldMetaInfo();
    field->name = fieldName;
    field->jvmName = NameTransformer::encode(fieldName);
    field->dataType = type;
    field->isVal = isVal;
    field->classMetaInfo = this;
    field->modifiers = mods;
    field->isInit = false;

    this->fields[fieldName] = field;
    return field;
}

optional<MethodMetaInfo *> ClassMetaInfo::addMethod(FunDefNode *funDefNode, Modifiers modifiers) {
    if (!funDefNode || !funDefNode->funSig || !funDefNode->funSig->fullId) return nullopt;

    string methodName = funDefNode->funSig->fullId->name;

    // Сбор временных аргументов для получения их типов
    vector<ArgMetaInfo *> tempArgs;
    vector<DataType *> signatureTypes;

    int tempCounter = 0;
    if (funDefNode->funSig->params && funDefNode->funSig->params->funcParams) {
        for (auto *paramNode: *funDefNode->funSig->params->funcParams) {
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

    // Проверка дубликатов - возвращаем nullopt, вызывающая сторона добавит ошибку
    if (this->resolveMethod(methodName, signatureTypes, this).has_value()) {
        for (auto *a: tempArgs) delete a;
        return nullopt;
    }

    // Метод уникален, создаем MetaInfo
    MethodMetaInfo *method = new MethodMetaInfo();
    method->name = methodName;
    method->jvmName = NameTransformer::encode(methodName);
    method->classMetaInfo = this;
    method->body = funDefNode->getBody();
    method->returnType = DataType::createFromNode(funDefNode->simpleType);
    method->modifiers = modifiers;
    method->localVarCounter = tempCounter;
    method->isPrimaryConstructor = funDefNode->primaryConstructor;

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

    // Проверка дубликатов - возвращаем nullopt, вызывающая сторона добавит ошибку
    if (this->resolveMethod(methodName, signatureTypes, this).has_value()) {
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

LocalVarMetaInfo::LocalVarMetaInfo() : MethodVarMetaInfo() {
    scope = nullptr;
}

std::string VarMetaInfo::toString() {
    return name + ": " + dataType.toString();
}

void VarMetaInfo::setDataType(DataType type) {
    dataType = type;
}

bool VarMetaInfo::operator==(const VarMetaInfo& other) {
    return name == other.name && dataType == other.dataType;
}

RtlClassMetaInfo* RtlClassMetaInfo::Any = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::String = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Integer = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::StdIn = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Predef = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Unit = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Char = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Double = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Boolean = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Array = nullptr;

void RtlClassMetaInfo::initializeRtlClasses() {
    // Инициализируем RTL классы в правильном порядке (учитываем зависимости)
    Any = RtlClassMetaInfo::initAny();
    ctx().addRTL(Any);

    String = RtlClassMetaInfo::initString();
    ctx().addRTL(String);

    Integer = RtlClassMetaInfo::initInteger();
    ctx().addRTL(Integer);

    Double = RtlClassMetaInfo::initDouble();
    ctx().addRTL(Double);

    Boolean = RtlClassMetaInfo::initBoolean();
    ctx().addRTL(Boolean);

    Char = RtlClassMetaInfo::initChar();
    ctx().addRTL(Char);

    Unit = RtlClassMetaInfo::initUnit();
    ctx().addRTL(Unit);

    StdIn = RtlClassMetaInfo::initStdIn();
    ctx().addRTL(StdIn);

    Predef = RtlClassMetaInfo::initPredef();
    ctx().addRTL(Predef);

    Array = RtlClassMetaInfo::initArray();
    ctx().addRTL(Array);

    std::cout << "RTL classes initialized successfully." << std::endl;
}

RtlClassMetaInfo* RtlClassMetaInfo::getRtlClassInfo(const std::string& typeName) {
    // Ищем в контексте
    auto it = ctx().classes.find(typeName);
    if (it != ctx().classes.end()) {
        // Проверяем, является ли это RTL классом
        return dynamic_cast<RtlClassMetaInfo*>(it->second);
    }
    return nullptr;
}

RtlClassMetaInfo *RtlClassMetaInfo::initAny() {
    string className = "Any";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _SCALA);
    rec->jvmName = "java/lang/Object";  // Any maps to java.lang.Object in JVM

    rec->parent = nullptr;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_ABSTRACT);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* toScalaString = new MethodMetaInfo();
    toScalaString->classMetaInfo = rec;
    toScalaString->modifiers.modifiers.push_back(_PUBLIC);
    toScalaString->returnType = DataType::Kind::String;
    toScalaString->name = "toScalaString";
    toScalaString->jvmName = NameTransformer::encode(toScalaString->name);
    toScalaString->args = vector<ArgMetaInfo*>();
    rec->methods[toScalaString->name].push_back(toScalaString);

    MethodMetaInfo* isInstanceOf = new MethodMetaInfo();
    isInstanceOf->classMetaInfo = rec;
    isInstanceOf->modifiers.modifiers.push_back(_PUBLIC);
    isInstanceOf->returnType = DataType::Kind::Bool;
    isInstanceOf->name = "isInstanceOf";
    isInstanceOf->jvmName = NameTransformer::encode(isInstanceOf->name);
    isInstanceOf->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* isInstanceOfArg1= new ArgMetaInfo();
    isInstanceOfArg1->name = "cls";
    isInstanceOfArg1->jvmName = NameTransformer::encode(isInstanceOfArg1->name);
    isInstanceOfArg1->dataType = DataType::Kind::Any;
    isInstanceOf->args.push_back(isInstanceOfArg1);
    rec->methods[isInstanceOf->name].push_back(isInstanceOf);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg = new ArgMetaInfo();
    equalsArg->name = "o";
    equalsArg->jvmName = NameTransformer::encode(equalsArg->name);
    equalsArg->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    return rec;
}

RtlClassMetaInfo* RtlClassMetaInfo::initString() {
    string className = "String";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _JAVA);
    rec->jvmName = "rtl/String";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* constructorArg = new ArgMetaInfo();
    constructorArg->name = "value";
    constructorArg->jvmName = NameTransformer::encode(constructorArg->name);
    constructorArg->dataType = DataType::Kind::String;
    constructor->args.push_back(constructorArg);
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* nativeValue = new MethodMetaInfo();
    nativeValue->classMetaInfo = rec;
    nativeValue->modifiers.modifiers.push_back(_PUBLIC);
    nativeValue->returnType = DataType::Kind::String;
    nativeValue->name = "nativeValue";
    nativeValue->jvmName = NameTransformer::encode(nativeValue->name);
    nativeValue->args = vector<ArgMetaInfo*>();
    rec->methods[nativeValue->name].push_back(nativeValue);

    MethodMetaInfo* length = new MethodMetaInfo();
    length->classMetaInfo = rec;
    length->modifiers.modifiers.push_back(_PUBLIC);
    length->returnType = DataType::Kind::Int;
    length->name = "length";
    length->jvmName = NameTransformer::encode(length->name);
    length->args = vector<ArgMetaInfo*>();
    rec->methods[length->name].push_back(length);

    MethodMetaInfo* charAt = new MethodMetaInfo();
    charAt->classMetaInfo = rec;
    charAt->modifiers.modifiers.push_back(_PUBLIC);
    charAt->returnType = DataType::Kind::Char;
    charAt->name = "charAt";
    charAt->jvmName = NameTransformer::encode(charAt->name);
    charAt->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* charAtArg = new ArgMetaInfo();
    charAtArg->name = "idx";
    charAtArg->jvmName = NameTransformer::encode(charAtArg->name);
    charAtArg->dataType = DataType::Kind::Int;
    charAt->args.push_back(charAtArg);
    rec->methods[charAt->name].push_back(charAt);

    MethodMetaInfo* concat = new MethodMetaInfo();
    concat->classMetaInfo = rec;
    concat->modifiers.modifiers.push_back(_PUBLIC);
    concat->returnType = DataType::Kind::String;
    concat->name = "concat";
    concat->jvmName = NameTransformer::encode(concat->name);
    concat->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* concatArg = new ArgMetaInfo();
    concatArg->name = "other";
    concatArg->jvmName = NameTransformer::encode(concatArg->name);
    concatArg->dataType = DataType::Kind::String;
    concat->args.push_back(concatArg);
    rec->methods[concat->name].push_back(concat);

    MethodMetaInfo* substring = new MethodMetaInfo();
    substring->classMetaInfo = rec;
    substring->modifiers.modifiers.push_back(_PUBLIC);
    substring->returnType = DataType::Kind::String;
    substring->name = "substring";
    substring->jvmName = NameTransformer::encode(substring->name);
    substring->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* substringArg1 = new ArgMetaInfo();
    substringArg1->name = "fromInclusive";
    substringArg1->jvmName = NameTransformer::encode(substringArg1->name);
    substringArg1->dataType = DataType::Kind::Int;
    substring->args.push_back(substringArg1);
    ArgMetaInfo* substringArg2 = new ArgMetaInfo();
    substringArg2->name = "toExclusive";
    substringArg2->jvmName = NameTransformer::encode(substringArg2->name);
    substringArg2->dataType = DataType::Kind::Int;
    substring->args.push_back(substringArg2);
    rec->methods[substring->name].push_back(substring);

    MethodMetaInfo* toInt = new MethodMetaInfo();
    toInt->classMetaInfo = rec;
    toInt->modifiers.modifiers.push_back(_PUBLIC);
    toInt->returnType = DataType::Kind::Int;
    toInt->name = "toInt";
    toInt->jvmName = NameTransformer::encode(toInt->name);
    toInt->args = vector<ArgMetaInfo*>();
    rec->methods[toInt->name].push_back(toInt);

    MethodMetaInfo* toFloat = new MethodMetaInfo();
    toFloat->classMetaInfo = rec;
    toFloat->modifiers.modifiers.push_back(_PUBLIC);
    toFloat->returnType = DataType::Kind::Double;
    toFloat->name = "toFloat";
    toFloat->jvmName = NameTransformer::encode(toFloat->name);
    toFloat->args = vector<ArgMetaInfo*>();
    rec->methods[toFloat->name].push_back(toFloat);

    MethodMetaInfo* toChar = new MethodMetaInfo();
    toChar->classMetaInfo = rec;
    toChar->modifiers.modifiers.push_back(_PUBLIC);
    toChar->returnType = DataType::Kind::Char;
    toChar->name = "toChar";
    toChar->jvmName = NameTransformer::encode(toChar->name);
    toChar->args = vector<ArgMetaInfo*>();
    rec->methods[toChar->name].push_back(toChar);

    MethodMetaInfo* equalsString = new MethodMetaInfo();
    equalsString->classMetaInfo = rec;
    equalsString->modifiers.modifiers.push_back(_PUBLIC);
    equalsString->returnType = DataType::Kind::Bool;
    equalsString->name = "equalsString";
    equalsString->jvmName = NameTransformer::encode(equalsString->name);
    equalsString->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsStringArg = new ArgMetaInfo();
    equalsStringArg->name = "other";
    equalsStringArg->jvmName = NameTransformer::encode(equalsStringArg->name);
    equalsStringArg->dataType = DataType::Kind::String;
    equalsString->args.push_back(equalsStringArg);
    rec->methods[equalsString->name].push_back(equalsString);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg = new ArgMetaInfo();
    equalsArg->name = "o";
    equalsArg->jvmName = NameTransformer::encode(equalsArg->name);
    equalsArg->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = rec;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->modifiers.modifiers.push_back(_OVERRIDE);
    toString->returnType = DataType::Kind::String;
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    return rec;
}

RtlClassMetaInfo* RtlClassMetaInfo::initInteger() {
    string className = "Int";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _JAVA);
    rec->jvmName = "rtl/Int";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* constructorArg = new ArgMetaInfo();
    constructorArg->name = "value";
    constructorArg->jvmName = NameTransformer::encode(constructorArg->name);
    constructorArg->dataType = DataType::Kind::Int;
    constructor->args.push_back(constructorArg);
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* intValue = new MethodMetaInfo();
    intValue->classMetaInfo = rec;
    intValue->modifiers.modifiers.push_back(_PUBLIC);
    intValue->returnType = DataType::Kind::Int;
    intValue->name = "intValue";
    intValue->jvmName = NameTransformer::encode(intValue->name);
    intValue->args = vector<ArgMetaInfo*>();
    rec->methods[intValue->name].push_back(intValue);

    MethodMetaInfo* add = new MethodMetaInfo();
    add->classMetaInfo = rec;
    add->modifiers.modifiers.push_back(_PUBLIC);
    add->returnType = DataType::Kind::Int;
    add->name = "add";
    add->jvmName = NameTransformer::encode(add->name);
    add->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* addArg = new ArgMetaInfo();
    addArg->name = "other";
    addArg->jvmName = NameTransformer::encode(addArg->name);
    addArg->dataType = DataType::Kind::Int;
    add->args.push_back(addArg);
    rec->methods[add->name].push_back(add);

    MethodMetaInfo* $plus = new MethodMetaInfo();
    $plus->classMetaInfo = rec;
    $plus->modifiers.modifiers.push_back(_PUBLIC);
    $plus->returnType = DataType::Kind::Int;
    $plus->name = "+";
    $plus->jvmName = NameTransformer::encode($plus->name);
    $plus->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $plusArg = new ArgMetaInfo();
    $plusArg->name = "other";
    $plusArg->jvmName = NameTransformer::encode($plusArg->name);
    $plusArg->dataType = DataType::Kind::Int;
    $plus->args.push_back($plusArg);
    rec->methods[$plus->name].push_back($plus);

    MethodMetaInfo* sub = new MethodMetaInfo();
    sub->classMetaInfo = rec;
    sub->modifiers.modifiers.push_back(_PUBLIC);
    sub->returnType = DataType::Kind::Int;
    sub->name = "sub";
    sub->jvmName = NameTransformer::encode(sub->name);
    sub->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* subArg = new ArgMetaInfo();
    subArg->name = "other";
    subArg->jvmName = NameTransformer::encode(subArg->name);
    subArg->dataType = DataType::Kind::Int;
    sub->args.push_back(subArg);
    rec->methods[sub->name].push_back(sub);

    MethodMetaInfo* $minus = new MethodMetaInfo();
    $minus->classMetaInfo = rec;
    $minus->modifiers.modifiers.push_back(_PUBLIC);
    $minus->returnType = DataType::Kind::Int;
    $minus->name = "-";
    $minus->jvmName = NameTransformer::encode($minus->name);
    $minus->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $minusArg = new ArgMetaInfo();
    $minusArg->name = "other";
    $minusArg->jvmName = NameTransformer::encode($minusArg->name);
    $minusArg->dataType = DataType::Kind::Int;
    $minus->args.push_back($minusArg);
    rec->methods[$minus->name].push_back($minus);

    MethodMetaInfo* mul = new MethodMetaInfo();
    mul->classMetaInfo = rec;
    mul->modifiers.modifiers.push_back(_PUBLIC);
    mul->returnType = DataType::Kind::Int;
    mul->name = "mul";
    mul->jvmName = NameTransformer::encode(mul->name);
    mul->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* mulArg = new ArgMetaInfo();
    mulArg->name = "other";
    mulArg->jvmName = NameTransformer::encode(mulArg->name);
    mulArg->dataType = DataType::Kind::Int;
    mul->args.push_back(mulArg);
    rec->methods[mul->name].push_back(mul);

    MethodMetaInfo* $times = new MethodMetaInfo();
    $times->classMetaInfo = rec;
    $times->modifiers.modifiers.push_back(_PUBLIC);
    $times->returnType = DataType::Kind::Int;
    $times->name = "*";
    $times->jvmName = NameTransformer::encode($times->name);
    $times->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $timesArg = new ArgMetaInfo();
    $timesArg->name = "other";
    $timesArg->jvmName = NameTransformer::encode($timesArg->name);
    $timesArg->dataType = DataType::Kind::Int;
    $times->args.push_back($timesArg);
    rec->methods[$times->name].push_back($times);

    MethodMetaInfo* div = new MethodMetaInfo();
    div->classMetaInfo = rec;
    div->modifiers.modifiers.push_back(_PUBLIC);
    div->returnType = DataType::Kind::Int;
    div->name = "div";
    div->jvmName = NameTransformer::encode(div->name);
    div->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* divArg = new ArgMetaInfo();
    divArg->name = "other";
    divArg->jvmName = NameTransformer::encode(divArg->name);
    divArg->dataType = DataType::Kind::Int;
    div->args.push_back(divArg);
    rec->methods[div->name].push_back(div);

    MethodMetaInfo* $div = new MethodMetaInfo();
    $div->classMetaInfo = rec;
    $div->modifiers.modifiers.push_back(_PUBLIC);
    $div->returnType = DataType::Kind::Int;
    $div->name = "/";
    $div->jvmName = NameTransformer::encode($div->name);
    $div->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $divArg = new ArgMetaInfo();
    $divArg->name = "other";
    $divArg->jvmName = NameTransformer::encode($divArg->name);
    $divArg->dataType = DataType::Kind::Int;
    $div->args.push_back($divArg);
    rec->methods[$div->name].push_back($div);

    MethodMetaInfo* mod = new MethodMetaInfo();
    mod->classMetaInfo = rec;
    mod->modifiers.modifiers.push_back(_PUBLIC);
    mod->returnType = DataType::Kind::Int;
    mod->name = "mod";
    mod->jvmName = NameTransformer::encode(mod->name);
    mod->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* modArg = new ArgMetaInfo();
    modArg->name = "other";
    modArg->jvmName = NameTransformer::encode(modArg->name);
    modArg->dataType = DataType::Kind::Int;
    mod->args.push_back(modArg);
    rec->methods[mod->name].push_back(mod);

    MethodMetaInfo* $percent = new MethodMetaInfo();
    $percent->classMetaInfo = rec;
    $percent->modifiers.modifiers.push_back(_PUBLIC);
    $percent->returnType = DataType::Kind::Int;
    $percent->name = "%";
    $percent->jvmName = NameTransformer::encode($percent->name);
    $percent->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $percentArg = new ArgMetaInfo();
    $percentArg->name = "other";
    $percentArg->jvmName = NameTransformer::encode($percentArg->name);
    $percentArg->dataType = DataType::Kind::Int;
    $percent->args.push_back($percentArg);
    rec->methods[$percent->name].push_back($percent);

    MethodMetaInfo* neg = new MethodMetaInfo();
    neg->classMetaInfo = rec;
    neg->modifiers.modifiers.push_back(_PUBLIC);
    neg->returnType = DataType::Kind::Int;
    neg->name = "neg";
    neg->jvmName = NameTransformer::encode(neg->name);
    neg->args = vector<ArgMetaInfo*>();
    rec->methods[neg->name].push_back(neg);

    MethodMetaInfo* lt = new MethodMetaInfo();
    lt->classMetaInfo = rec;
    lt->modifiers.modifiers.push_back(_PUBLIC);
    lt->returnType = DataType::Kind::Bool;
    lt->name = "lt";
    lt->jvmName = NameTransformer::encode(lt->name);
    lt->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* ltArg = new ArgMetaInfo();
    ltArg->name = "other";
    ltArg->jvmName = NameTransformer::encode(ltArg->name);
    ltArg->dataType = DataType::Kind::Int;
    lt->args.push_back(ltArg);
    rec->methods[lt->name].push_back(lt);

    MethodMetaInfo* $less = new MethodMetaInfo();
    $less->classMetaInfo = rec;
    $less->modifiers.modifiers.push_back(_PUBLIC);
    $less->returnType = DataType::Kind::Bool;
    $less->name = "<";
    $less->jvmName = NameTransformer::encode($less->name);
    $less->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $lessArg = new ArgMetaInfo();
    $lessArg->name = "other";
    $lessArg->jvmName = NameTransformer::encode($lessArg->name);
    $lessArg->dataType = DataType::Kind::Int;
    $less->args.push_back($lessArg);
    rec->methods[$less->name].push_back($less);

    MethodMetaInfo* le = new MethodMetaInfo();
    le->classMetaInfo = rec;
    le->modifiers.modifiers.push_back(_PUBLIC);
    le->returnType = DataType::Kind::Bool;
    le->name = "le";
    le->jvmName = NameTransformer::encode(le->name);
    le->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* leArg = new ArgMetaInfo();
    leArg->name = "other";
    leArg->jvmName = NameTransformer::encode(leArg->name);
    leArg->dataType = DataType::Kind::Int;
    le->args.push_back(leArg);
    rec->methods[le->name].push_back(le);

    MethodMetaInfo* $less$eq = new MethodMetaInfo();
    $less$eq->classMetaInfo = rec;
    $less$eq->modifiers.modifiers.push_back(_PUBLIC);
    $less$eq->returnType = DataType::Kind::Bool;
    $less$eq->name = "<=";
    $less$eq->jvmName = NameTransformer::encode($less$eq->name);
    $less$eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $less$eqArg = new ArgMetaInfo();
    $less$eqArg->name = "other";
    $less$eqArg->jvmName = NameTransformer::encode($less$eqArg->name);
    $less$eqArg->dataType = DataType::Kind::Int;
    $less$eq->args.push_back($less$eqArg);
    rec->methods[$less$eq->name].push_back($less$eq);

    MethodMetaInfo* gt = new MethodMetaInfo();
    gt->classMetaInfo = rec;
    gt->modifiers.modifiers.push_back(_PUBLIC);
    gt->returnType = DataType::Kind::Bool;
    gt->name = "gt";
    gt->jvmName = NameTransformer::encode(gt->name);
    gt->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* gtArg = new ArgMetaInfo();
    gtArg->name = "other";
    gtArg->jvmName = NameTransformer::encode(gtArg->name);
    gtArg->dataType = DataType::Kind::Int;
    gt->args.push_back(gtArg);
    rec->methods[gt->name].push_back(gt);

    MethodMetaInfo* $greater = new MethodMetaInfo();
    $greater->classMetaInfo = rec;
    $greater->modifiers.modifiers.push_back(_PUBLIC);
    $greater->returnType = DataType::Kind::Bool;
    $greater->name = ">";
    $greater->jvmName = NameTransformer::encode($greater->name);
    $greater->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $greaterArg = new ArgMetaInfo();
    $greaterArg->name = "other";
    $greaterArg->jvmName = NameTransformer::encode($greaterArg->name);
    $greaterArg->dataType = DataType::Kind::Int;
    $greater->args.push_back($greaterArg);
    rec->methods[$greater->name].push_back($greater);

    MethodMetaInfo* ge = new MethodMetaInfo();
    ge->classMetaInfo = rec;
    ge->modifiers.modifiers.push_back(_PUBLIC);
    ge->returnType = DataType::Kind::Bool;
    ge->name = "ge";
    ge->jvmName = NameTransformer::encode(ge->name);
    ge->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* geArg = new ArgMetaInfo();
    geArg->name = "other";
    geArg->jvmName = NameTransformer::encode(geArg->name);
    geArg->dataType = DataType::Kind::Int;
    ge->args.push_back(geArg);
    rec->methods[ge->name].push_back(ge);

    MethodMetaInfo* $greater$eq = new MethodMetaInfo();
    $greater$eq->classMetaInfo = rec;
    $greater$eq->modifiers.modifiers.push_back(_PUBLIC);
    $greater$eq->returnType = DataType::Kind::Bool;
    $greater$eq->name = ">=";
    $greater$eq->jvmName = NameTransformer::encode($greater$eq->name);
    $greater$eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $greater$eqArg = new ArgMetaInfo();
    $greater$eqArg->name = "other";
    $greater$eqArg->jvmName = NameTransformer::encode($greater$eqArg->name);
    $greater$eqArg->dataType = DataType::Kind::Int;
    $greater$eq->args.push_back($greater$eqArg);
    rec->methods[$greater$eq->name].push_back($greater$eq);

    MethodMetaInfo* eq = new MethodMetaInfo();
    eq->classMetaInfo = rec;
    eq->modifiers.modifiers.push_back(_PUBLIC);
    eq->returnType = DataType::Kind::Bool;
    eq->name = "eq";
    eq->jvmName = NameTransformer::encode(eq->name);
    eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* eqArg = new ArgMetaInfo();
    eqArg->name = "other";
    eqArg->jvmName = NameTransformer::encode(eqArg->name);
    eqArg->dataType = DataType::Kind::Int;
    eq->args.push_back(eqArg);
    rec->methods[eq->name].push_back(eq);

    MethodMetaInfo* $eq$eq = new MethodMetaInfo();
    $eq$eq->classMetaInfo = rec;
    $eq$eq->modifiers.modifiers.push_back(_PUBLIC);
    $eq$eq->returnType = DataType::Kind::Bool;
    $eq$eq->name = "==";
    $eq$eq->jvmName = NameTransformer::encode($eq$eq->name);
    $eq$eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $eq$eqArg = new ArgMetaInfo();
    $eq$eqArg->name = "other";
    $eq$eqArg->jvmName = NameTransformer::encode($eq$eqArg->name);
    $eq$eqArg->dataType = DataType::Kind::Int;
    $eq$eq->args.push_back($eq$eqArg);
    rec->methods[$eq$eq->name].push_back($eq$eq);

    MethodMetaInfo* ne = new MethodMetaInfo();
    ne->classMetaInfo = rec;
    ne->modifiers.modifiers.push_back(_PUBLIC);
    ne->returnType = DataType::Kind::Bool;
    ne->name = "ne";
    ne->jvmName = NameTransformer::encode(ne->name);
    ne->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* neArg = new ArgMetaInfo();
    neArg->name = "other";
    neArg->jvmName = NameTransformer::encode(neArg->name);
    neArg->dataType = DataType::Kind::Int;
    ne->args.push_back(neArg);
    rec->methods[ne->name].push_back(ne);

    MethodMetaInfo* $bang$eq = new MethodMetaInfo();
    $bang$eq->classMetaInfo = rec;
    $bang$eq->modifiers.modifiers.push_back(_PUBLIC);
    $bang$eq->returnType = DataType::Kind::Bool;
    $bang$eq->name = "!=";
    $bang$eq->jvmName = NameTransformer::encode($bang$eq->name);
    $bang$eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $bang$eqArg = new ArgMetaInfo();
    $bang$eqArg->name = "other";
    $bang$eqArg->jvmName = NameTransformer::encode($bang$eqArg->name);
    $bang$eqArg->dataType = DataType::Kind::Int;
    $bang$eq->args.push_back($bang$eqArg);
    rec->methods[$bang$eq->name].push_back($bang$eq);

    MethodMetaInfo* toFloat = new MethodMetaInfo();
    toFloat->classMetaInfo = rec;
    toFloat->modifiers.modifiers.push_back(_PUBLIC);
    toFloat->returnType = DataType::Kind::Double;
    toFloat->name = "toFloat";
    toFloat->jvmName = NameTransformer::encode(toFloat->name);
    toFloat->args = vector<ArgMetaInfo*>();
    rec->methods[toFloat->name].push_back(toFloat);

    MethodMetaInfo* toChar = new MethodMetaInfo();
    toChar->classMetaInfo = rec;
    toChar->modifiers.modifiers.push_back(_PUBLIC);
    toChar->returnType = DataType::Kind::Char;
    toChar->name = "toChar";
    toChar->jvmName = NameTransformer::encode(toChar->name);
    toChar->args = vector<ArgMetaInfo*>();
    rec->methods[toChar->name].push_back(toChar);

    MethodMetaInfo* toStringValue = new MethodMetaInfo();
    toStringValue->classMetaInfo = rec;
    toStringValue->modifiers.modifiers.push_back(_PUBLIC);
    toStringValue->returnType = DataType::Kind::String;
    toStringValue->name = "toStringValue";
    toStringValue->jvmName = NameTransformer::encode(toStringValue->name);
    toStringValue->args = vector<ArgMetaInfo*>();
    rec->methods[toStringValue->name].push_back(toStringValue);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg = new ArgMetaInfo();
    equalsArg->name = "o";
    equalsArg->jvmName = NameTransformer::encode(equalsArg->name);
    equalsArg->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = rec;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->modifiers.modifiers.push_back(_OVERRIDE);
    toString->returnType = DataType::Kind::String;
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    // Унарные операторы
    MethodMetaInfo* unary_plus = new MethodMetaInfo();
    unary_plus->classMetaInfo = rec;
    unary_plus->modifiers.modifiers.push_back(_PUBLIC);
    unary_plus->returnType = DataType::Kind::Int;
    unary_plus->name = "unary_+";
    unary_plus->jvmName = NameTransformer::encode(unary_plus->name);
    unary_plus->args = vector<ArgMetaInfo*>();
    rec->methods[unary_plus->name].push_back(unary_plus);

    MethodMetaInfo* unary_minus = new MethodMetaInfo();
    unary_minus->classMetaInfo = rec;
    unary_minus->modifiers.modifiers.push_back(_PUBLIC);
    unary_minus->returnType = DataType::Kind::Int;
    unary_minus->name = "unary_-";
    unary_minus->jvmName = NameTransformer::encode(unary_minus->name);
    unary_minus->args = vector<ArgMetaInfo*>();
    rec->methods[unary_minus->name].push_back(unary_minus);

    MethodMetaInfo* unary_tilde = new MethodMetaInfo();
    unary_tilde->classMetaInfo = rec;
    unary_tilde->modifiers.modifiers.push_back(_PUBLIC);
    unary_tilde->returnType = DataType::Kind::Int;
    unary_tilde->name = "unary_~";
    unary_tilde->jvmName = NameTransformer::encode(unary_tilde->name);
    unary_tilde->args = vector<ArgMetaInfo*>();
    rec->methods[unary_tilde->name].push_back(unary_tilde);

    return rec;
}

RtlClassMetaInfo* RtlClassMetaInfo::initStdIn() {
    string className = "StdIn";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _SCALA);
    rec->jvmName = "rtl/StdIn";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* readLine = new MethodMetaInfo();
    readLine->classMetaInfo = rec;
    readLine->modifiers.modifiers.push_back(_PUBLIC);
    readLine->returnType = DataType::Kind::String;
    readLine->name = "readLine";
    readLine->jvmName = NameTransformer::encode(readLine->name);
    readLine->args = vector<ArgMetaInfo*>();
    rec->methods[readLine->name].push_back(readLine);

    MethodMetaInfo* readBoolean = new MethodMetaInfo();
    readBoolean->classMetaInfo = rec;
    readBoolean->modifiers.modifiers.push_back(_PUBLIC);
    readBoolean->returnType = DataType::Kind::Bool;
    readBoolean->name = "readBoolean";
    readBoolean->jvmName = NameTransformer::encode(readBoolean->name);
    readBoolean->args = vector<ArgMetaInfo*>();
    rec->methods[readBoolean->name].push_back(readBoolean);

    MethodMetaInfo* readChar = new MethodMetaInfo();
    readChar->classMetaInfo = rec;
    readChar->modifiers.modifiers.push_back(_PUBLIC);
    readChar->returnType = DataType::Kind::Char;
    readChar->name = "readChar";
    readChar->jvmName = NameTransformer::encode(readChar->name);
    readChar->args = vector<ArgMetaInfo*>();
    rec->methods[readChar->name].push_back(readChar);

    MethodMetaInfo* readInt = new MethodMetaInfo();
    readInt->classMetaInfo = rec;
    readInt->modifiers.modifiers.push_back(_PUBLIC);
    readInt->returnType = DataType::Kind::Int;
    readInt->name = "readInt";
    readInt->jvmName = NameTransformer::encode(readInt->name);
    readInt->args = vector<ArgMetaInfo*>();
    rec->methods[readInt->name].push_back(readInt);

    MethodMetaInfo* readDouble = new MethodMetaInfo();
    readDouble->classMetaInfo = rec;
    readDouble->modifiers.modifiers.push_back(_PUBLIC);
    readDouble->returnType = DataType::Kind::Double;
    readDouble->name = "readDouble";
    readDouble->jvmName = NameTransformer::encode(readDouble->name);
    readDouble->args = vector<ArgMetaInfo*>();
    rec->methods[readDouble->name].push_back(readDouble);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg = new ArgMetaInfo();
    equalsArg->name = "o";
    equalsArg->jvmName = NameTransformer::encode(equalsArg->name);
    equalsArg->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    return rec;
}

RtlClassMetaInfo* RtlClassMetaInfo::initUnit() {
    string className = "Unit";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _JAVA);
    rec->jvmName = "rtl/Unit";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg = new ArgMetaInfo();
    equalsArg->name = "o";
    equalsArg->jvmName = NameTransformer::encode(equalsArg->name);
    equalsArg->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = rec;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->modifiers.modifiers.push_back(_OVERRIDE);
    toString->returnType = DataType::Kind::String;
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    MethodMetaInfo* toScalaString = new MethodMetaInfo();
    toScalaString->classMetaInfo = rec;
    toScalaString->modifiers.modifiers.push_back(_PUBLIC);
    toScalaString->modifiers.modifiers.push_back(_OVERRIDE);
    toScalaString->returnType = DataType::Kind::String;
    toScalaString->name = "toScalaString";
    toScalaString->jvmName = NameTransformer::encode(toScalaString->name);
    toScalaString->args = vector<ArgMetaInfo*>();
    rec->methods[toScalaString->name].push_back(toScalaString);

    return rec;
}

RtlClassMetaInfo* RtlClassMetaInfo::initChar() {
    string className = "Char";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _JAVA);
    rec->jvmName = "rtl/Char";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* constructorArg = new ArgMetaInfo();
    constructorArg->name = "value";
    constructorArg->jvmName = NameTransformer::encode(constructorArg->name);
    constructorArg->dataType = DataType::Kind::Char;
    constructor->args.push_back(constructorArg);
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* charValue = new MethodMetaInfo();
    charValue->classMetaInfo = rec;
    charValue->modifiers.modifiers.push_back(_PUBLIC);
    charValue->returnType = DataType::Kind::Char;
    charValue->name = "charValue";
    charValue->jvmName = NameTransformer::encode(charValue->name);
    charValue->args = vector<ArgMetaInfo*>();
    rec->methods[charValue->name].push_back(charValue);

    MethodMetaInfo* toInt = new MethodMetaInfo();
    toInt->classMetaInfo = rec;
    toInt->modifiers.modifiers.push_back(_PUBLIC);
    toInt->returnType = DataType::Kind::Int;
    toInt->name = "toInt";
    toInt->jvmName = NameTransformer::encode(toInt->name);
    toInt->args = vector<ArgMetaInfo*>();
    rec->methods[toInt->name].push_back(toInt);

    MethodMetaInfo* toStringValue = new MethodMetaInfo();
    toStringValue->classMetaInfo = rec;
    toStringValue->modifiers.modifiers.push_back(_PUBLIC);
    toStringValue->returnType = DataType::Kind::String;
    toStringValue->name = "toStringValue";
    toStringValue->jvmName = NameTransformer::encode(toStringValue->name);
    toStringValue->args = vector<ArgMetaInfo*>();
    rec->methods[toStringValue->name].push_back(toStringValue);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg = new ArgMetaInfo();
    equalsArg->name = "o";
    equalsArg->jvmName = NameTransformer::encode(equalsArg->name);
    equalsArg->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = rec;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->modifiers.modifiers.push_back(_OVERRIDE);
    toString->returnType = DataType::Kind::String;
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    MethodMetaInfo* toScalaString = new MethodMetaInfo();
    toScalaString->classMetaInfo = rec;
    toScalaString->modifiers.modifiers.push_back(_PUBLIC);
    toScalaString->modifiers.modifiers.push_back(_OVERRIDE);
    toScalaString->returnType = DataType::Kind::String;
    toScalaString->name = "toScalaString";
    toScalaString->jvmName = NameTransformer::encode(toScalaString->name);
    toScalaString->args = vector<ArgMetaInfo*>();
    rec->methods[toScalaString->name].push_back(toScalaString);

    return rec;
}

RtlClassMetaInfo* RtlClassMetaInfo::initDouble() {
    string className = "Double";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _JAVA);
    rec->jvmName = "rtl/Double";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* constructorArg = new ArgMetaInfo();
    constructorArg->name = "value";
    constructorArg->jvmName = NameTransformer::encode(constructorArg->name);
    constructorArg->dataType = DataType::Kind::Double;
    constructor->args.push_back(constructorArg);
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* doubleValue = new MethodMetaInfo();
    doubleValue->classMetaInfo = rec;
    doubleValue->modifiers.modifiers.push_back(_PUBLIC);
    doubleValue->returnType = DataType::Kind::Double;
    doubleValue->name = "doubleValue";
    doubleValue->jvmName = NameTransformer::encode(doubleValue->name);
    doubleValue->args = vector<ArgMetaInfo*>();
    rec->methods[doubleValue->name].push_back(doubleValue);

    MethodMetaInfo* add = new MethodMetaInfo();
    add->classMetaInfo = rec;
    add->modifiers.modifiers.push_back(_PUBLIC);
    add->returnType = DataType::Kind::Double;
    add->name = "add";
    add->jvmName = NameTransformer::encode(add->name);
    add->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* addArg = new ArgMetaInfo();
    addArg->name = "other";
    addArg->jvmName = NameTransformer::encode(addArg->name);
    addArg->dataType = DataType::Kind::Double;
    add->args.push_back(addArg);
    rec->methods[add->name].push_back(add);

    MethodMetaInfo* $plus = new MethodMetaInfo();
    $plus->classMetaInfo = rec;
    $plus->modifiers.modifiers.push_back(_PUBLIC);
    $plus->returnType = DataType::Kind::Double;
    $plus->name = "+";
    $plus->jvmName = NameTransformer::encode($plus->name);
    $plus->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $plusArg = new ArgMetaInfo();
    $plusArg->name = "other";
    $plusArg->jvmName = NameTransformer::encode($plusArg->name);
    $plusArg->dataType = DataType::Kind::Double;
    $plus->args.push_back($plusArg);
    rec->methods[$plus->name].push_back($plus);

    MethodMetaInfo* sub = new MethodMetaInfo();
    sub->classMetaInfo = rec;
    sub->modifiers.modifiers.push_back(_PUBLIC);
    sub->returnType = DataType::Kind::Double;
    sub->name = "sub";
    sub->jvmName = NameTransformer::encode(sub->name);
    sub->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* subArg = new ArgMetaInfo();
    subArg->name = "other";
    subArg->jvmName = NameTransformer::encode(subArg->name);
    subArg->dataType = DataType::Kind::Double;
    sub->args.push_back(subArg);
    rec->methods[sub->name].push_back(sub);

    MethodMetaInfo* $minus = new MethodMetaInfo();
    $minus->classMetaInfo = rec;
    $minus->modifiers.modifiers.push_back(_PUBLIC);
    $minus->returnType = DataType::Kind::Double;
    $minus->name = "-";
    $minus->jvmName = NameTransformer::encode($minus->name);
    $minus->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $minusArg = new ArgMetaInfo();
    $minusArg->name = "other";
    $minusArg->jvmName = NameTransformer::encode($minusArg->name);
    $minusArg->dataType = DataType::Kind::Double;
    $minus->args.push_back($minusArg);
    rec->methods[$minus->name].push_back($minus);

    MethodMetaInfo* mul = new MethodMetaInfo();
    mul->classMetaInfo = rec;
    mul->modifiers.modifiers.push_back(_PUBLIC);
    mul->returnType = DataType::Kind::Double;
    mul->name = "mul";
    mul->jvmName = NameTransformer::encode(mul->name);
    mul->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* mulArg = new ArgMetaInfo();
    mulArg->name = "other";
    mulArg->jvmName = NameTransformer::encode(mulArg->name);
    mulArg->dataType = DataType::Kind::Double;
    mul->args.push_back(mulArg);
    rec->methods[mul->name].push_back(mul);

    MethodMetaInfo* $times = new MethodMetaInfo();
    $times->classMetaInfo = rec;
    $times->modifiers.modifiers.push_back(_PUBLIC);
    $times->returnType = DataType::Kind::Double;
    $times->name = "*";
    $times->jvmName = NameTransformer::encode($times->name);
    $times->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $timesArg = new ArgMetaInfo();
    $timesArg->name = "other";
    $timesArg->jvmName = NameTransformer::encode($timesArg->name);
    $timesArg->dataType = DataType::Kind::Double;
    $times->args.push_back($timesArg);
    rec->methods[$times->name].push_back($times);

    MethodMetaInfo* div = new MethodMetaInfo();
    div->classMetaInfo = rec;
    div->modifiers.modifiers.push_back(_PUBLIC);
    div->returnType = DataType::Kind::Double;
    div->name = "div";
    div->jvmName = NameTransformer::encode(div->name);
    div->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* divArg = new ArgMetaInfo();
    divArg->name = "other";
    divArg->jvmName = NameTransformer::encode(divArg->name);
    divArg->dataType = DataType::Kind::Double;
    div->args.push_back(divArg);
    rec->methods[div->name].push_back(div);

    MethodMetaInfo* $div = new MethodMetaInfo();
    $div->classMetaInfo = rec;
    $div->modifiers.modifiers.push_back(_PUBLIC);
    $div->returnType = DataType::Kind::Double;
    $div->name = "/";
    $div->jvmName = NameTransformer::encode($div->name);
    $div->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $divArg = new ArgMetaInfo();
    $divArg->name = "other";
    $divArg->jvmName = NameTransformer::encode($divArg->name);
    $divArg->dataType = DataType::Kind::Double;
    $div->args.push_back($divArg);
    rec->methods[$div->name].push_back($div);

    MethodMetaInfo* lt = new MethodMetaInfo();
    lt->classMetaInfo = rec;
    lt->modifiers.modifiers.push_back(_PUBLIC);
    lt->returnType = DataType::Kind::Bool;
    lt->name = "lt";
    lt->jvmName = NameTransformer::encode(lt->name);
    lt->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* ltArg = new ArgMetaInfo();
    ltArg->name = "other";
    ltArg->jvmName = NameTransformer::encode(ltArg->name);
    ltArg->dataType = DataType::Kind::Double;
    lt->args.push_back(ltArg);
    rec->methods[lt->name].push_back(lt);

    MethodMetaInfo* $less = new MethodMetaInfo();
    $less->classMetaInfo = rec;
    $less->modifiers.modifiers.push_back(_PUBLIC);
    $less->returnType = DataType::Kind::Bool;
    $less->name = "<";
    $less->jvmName = NameTransformer::encode($less->name);
    $less->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $lessArg = new ArgMetaInfo();
    $lessArg->name = "other";
    $lessArg->jvmName = NameTransformer::encode($lessArg->name);
    $lessArg->dataType = DataType::Kind::Double;
    $less->args.push_back($lessArg);
    rec->methods[$less->name].push_back($less);

    MethodMetaInfo* le = new MethodMetaInfo();
    le->classMetaInfo = rec;
    le->modifiers.modifiers.push_back(_PUBLIC);
    le->returnType = DataType::Kind::Bool;
    le->name = "le";
    le->jvmName = NameTransformer::encode(le->name);
    le->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* leArg = new ArgMetaInfo();
    leArg->name = "other";
    leArg->jvmName = NameTransformer::encode(leArg->name);
    leArg->dataType = DataType::Kind::Double;
    le->args.push_back(leArg);
    rec->methods[le->name].push_back(le);

    MethodMetaInfo* $less$eq = new MethodMetaInfo();
    $less$eq->classMetaInfo = rec;
    $less$eq->modifiers.modifiers.push_back(_PUBLIC);
    $less$eq->returnType = DataType::Kind::Bool;
    $less$eq->name = "<=";
    $less$eq->jvmName = NameTransformer::encode($less$eq->name);
    $less$eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $less$eqArg = new ArgMetaInfo();
    $less$eqArg->name = "other";
    $less$eqArg->jvmName = NameTransformer::encode($less$eqArg->name);
    $less$eqArg->dataType = DataType::Kind::Double;
    $less$eq->args.push_back($less$eqArg);
    rec->methods[$less$eq->name].push_back($less$eq);

    MethodMetaInfo* gt = new MethodMetaInfo();
    gt->classMetaInfo = rec;
    gt->modifiers.modifiers.push_back(_PUBLIC);
    gt->returnType = DataType::Kind::Bool;
    gt->name = "gt";
    gt->jvmName = NameTransformer::encode(gt->name);
    gt->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* gtArg = new ArgMetaInfo();
    gtArg->name = "other";
    gtArg->jvmName = NameTransformer::encode(gtArg->name);
    gtArg->dataType = DataType::Kind::Double;
    gt->args.push_back(gtArg);
    rec->methods[gt->name].push_back(gt);

    MethodMetaInfo* $greater = new MethodMetaInfo();
    $greater->classMetaInfo = rec;
    $greater->modifiers.modifiers.push_back(_PUBLIC);
    $greater->returnType = DataType::Kind::Bool;
    $greater->name = ">";
    $greater->jvmName = NameTransformer::encode($greater->name);
    $greater->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $greaterArg = new ArgMetaInfo();
    $greaterArg->name = "other";
    $greaterArg->jvmName = NameTransformer::encode($greaterArg->name);
    $greaterArg->dataType = DataType::Kind::Double;
    $greater->args.push_back($greaterArg);
    rec->methods[$greater->name].push_back($greater);

    MethodMetaInfo* ge = new MethodMetaInfo();
    ge->classMetaInfo = rec;
    ge->modifiers.modifiers.push_back(_PUBLIC);
    ge->returnType = DataType::Kind::Bool;
    ge->name = "ge";
    ge->jvmName = NameTransformer::encode(ge->name);
    ge->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* geArg = new ArgMetaInfo();
    geArg->name = "other";
    geArg->jvmName = NameTransformer::encode(geArg->name);
    geArg->dataType = DataType::Kind::Double;
    ge->args.push_back(geArg);
    rec->methods[ge->name].push_back(ge);

    MethodMetaInfo* $greater$eq = new MethodMetaInfo();
    $greater$eq->classMetaInfo = rec;
    $greater$eq->modifiers.modifiers.push_back(_PUBLIC);
    $greater$eq->returnType = DataType::Kind::Bool;
    $greater$eq->name = ">=";
    $greater$eq->jvmName = NameTransformer::encode($greater$eq->name);
    $greater$eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $greater$eqArg = new ArgMetaInfo();
    $greater$eqArg->name = "other";
    $greater$eqArg->jvmName = NameTransformer::encode($greater$eqArg->name);
    $greater$eqArg->dataType = DataType::Kind::Double;
    $greater$eq->args.push_back($greater$eqArg);
    rec->methods[$greater$eq->name].push_back($greater$eq);

    MethodMetaInfo* eq = new MethodMetaInfo();
    eq->classMetaInfo = rec;
    eq->modifiers.modifiers.push_back(_PUBLIC);
    eq->returnType = DataType::Kind::Bool;
    eq->name = "eq";
    eq->jvmName = NameTransformer::encode(eq->name);
    eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* eqArg = new ArgMetaInfo();
    eqArg->name = "other";
    eqArg->jvmName = NameTransformer::encode(eqArg->name);
    eqArg->dataType = DataType::Kind::Double;
    eq->args.push_back(eqArg);
    rec->methods[eq->name].push_back(eq);

    MethodMetaInfo* $eq$eq = new MethodMetaInfo();
    $eq$eq->classMetaInfo = rec;
    $eq$eq->modifiers.modifiers.push_back(_PUBLIC);
    $eq$eq->returnType = DataType::Kind::Bool;
    $eq$eq->name = "==";
    $eq$eq->jvmName = NameTransformer::encode($eq$eq->name);
    $eq$eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $eq$eqArg = new ArgMetaInfo();
    $eq$eqArg->name = "other";
    $eq$eqArg->jvmName = NameTransformer::encode($eq$eqArg->name);
    $eq$eqArg->dataType = DataType::Kind::Double;
    $eq$eq->args.push_back($eq$eqArg);
    rec->methods[$eq$eq->name].push_back($eq$eq);

    MethodMetaInfo* ne = new MethodMetaInfo();
    ne->classMetaInfo = rec;
    ne->modifiers.modifiers.push_back(_PUBLIC);
    ne->returnType = DataType::Kind::Bool;
    ne->name = "ne";
    ne->jvmName = NameTransformer::encode(ne->name);
    ne->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* neArg = new ArgMetaInfo();
    neArg->name = "other";
    neArg->jvmName = NameTransformer::encode(neArg->name);
    neArg->dataType = DataType::Kind::Double;
    ne->args.push_back(neArg);
    rec->methods[ne->name].push_back(ne);

    MethodMetaInfo* $bang$eq = new MethodMetaInfo();
    $bang$eq->classMetaInfo = rec;
    $bang$eq->modifiers.modifiers.push_back(_PUBLIC);
    $bang$eq->returnType = DataType::Kind::Bool;
    $bang$eq->name = "!=";
    $bang$eq->jvmName = NameTransformer::encode($bang$eq->name);
    $bang$eq->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $bang$eqArg = new ArgMetaInfo();
    $bang$eqArg->name = "other";
    $bang$eqArg->jvmName = NameTransformer::encode($bang$eqArg->name);
    $bang$eqArg->dataType = DataType::Kind::Double;
    $bang$eq->args.push_back($bang$eqArg);
    rec->methods[$bang$eq->name].push_back($bang$eq);

    MethodMetaInfo* toInt = new MethodMetaInfo();
    toInt->classMetaInfo = rec;
    toInt->modifiers.modifiers.push_back(_PUBLIC);
    toInt->returnType = DataType::Kind::Int;
    toInt->name = "toInt";
    toInt->jvmName = NameTransformer::encode(toInt->name);
    toInt->args = vector<ArgMetaInfo*>();
    rec->methods[toInt->name].push_back(toInt);

    MethodMetaInfo* toStringValue = new MethodMetaInfo();
    toStringValue->classMetaInfo = rec;
    toStringValue->modifiers.modifiers.push_back(_PUBLIC);
    toStringValue->returnType = DataType::Kind::String;
    toStringValue->name = "toStringValue";
    toStringValue->jvmName = NameTransformer::encode(toStringValue->name);
    toStringValue->args = vector<ArgMetaInfo*>();
    rec->methods[toStringValue->name].push_back(toStringValue);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg = new ArgMetaInfo();
    equalsArg->name = "o";
    equalsArg->jvmName = NameTransformer::encode(equalsArg->name);
    equalsArg->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = rec;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->modifiers.modifiers.push_back(_OVERRIDE);
    toString->returnType = DataType::Kind::String;
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    // Унарные операторы
    MethodMetaInfo* unary_plus_d = new MethodMetaInfo();
    unary_plus_d->classMetaInfo = rec;
    unary_plus_d->modifiers.modifiers.push_back(_PUBLIC);
    unary_plus_d->returnType = DataType::Kind::Double;
    unary_plus_d->name = "unary_+";
    unary_plus_d->jvmName = NameTransformer::encode(unary_plus_d->name);
    unary_plus_d->args = vector<ArgMetaInfo*>();
    rec->methods[unary_plus_d->name].push_back(unary_plus_d);

    MethodMetaInfo* unary_minus_d = new MethodMetaInfo();
    unary_minus_d->classMetaInfo = rec;
    unary_minus_d->modifiers.modifiers.push_back(_PUBLIC);
    unary_minus_d->returnType = DataType::Kind::Double;
    unary_minus_d->name = "unary_-";
    unary_minus_d->jvmName = NameTransformer::encode(unary_minus_d->name);
    unary_minus_d->args = vector<ArgMetaInfo*>();
    rec->methods[unary_minus_d->name].push_back(unary_minus_d);

    return rec;
}

RtlClassMetaInfo* RtlClassMetaInfo::initBoolean() {
    string className = "Boolean";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _JAVA);
    rec->jvmName = "rtl/Boolean";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* constructor1= new ArgMetaInfo();
    constructor1->name = "v";
    constructor1->jvmName = NameTransformer::encode(constructor1->name);
    constructor1->dataType = DataType::Kind::Bool;
    constructor->args.push_back(constructor1);
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* booleanValue = new MethodMetaInfo();
    booleanValue->classMetaInfo = rec;
    booleanValue->modifiers.modifiers.push_back(_PUBLIC);
    booleanValue->returnType = DataType::Kind::Bool;
    booleanValue->name = "booleanValue";
    booleanValue->jvmName = NameTransformer::encode(booleanValue->name);
    booleanValue->args = vector<ArgMetaInfo*>();
    rec->methods[booleanValue->name].push_back(booleanValue);

    MethodMetaInfo* andM = new MethodMetaInfo();
    andM->classMetaInfo = rec;
    andM->modifiers.modifiers.push_back(_PUBLIC);
    andM->returnType = DataType::Kind::Bool;
    andM->name = "and";
    andM->jvmName = NameTransformer::encode(andM->name);
    andM->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* andM1= new ArgMetaInfo();
    andM1->name = "other";
    andM1->jvmName = NameTransformer::encode(andM1->name);
    andM1->dataType = DataType::Kind::Bool;
    andM->args.push_back(andM1);
    rec->methods[andM->name].push_back(andM);

    MethodMetaInfo* $amp = new MethodMetaInfo();
    $amp->classMetaInfo = rec;
    $amp->modifiers.modifiers.push_back(_PUBLIC);
    $amp->returnType = DataType::Kind::Bool;
    $amp->name = "&";
    $amp->jvmName = NameTransformer::encode($amp->name);
    $amp->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $amp1= new ArgMetaInfo();
    $amp1->name = "other";
    $amp1->jvmName = NameTransformer::encode($amp1->name);
    $amp1->dataType = DataType::Kind::Bool;
    $amp->args.push_back($amp1);
    rec->methods[$amp->name].push_back($amp);

    MethodMetaInfo* $amp$amp = new MethodMetaInfo();
    $amp$amp->classMetaInfo = rec;
    $amp$amp->modifiers.modifiers.push_back(_PUBLIC);
    $amp$amp->returnType = DataType::Kind::Bool;
    $amp$amp->name = "&&";
    $amp$amp->jvmName = NameTransformer::encode($amp$amp->name);
    $amp$amp->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $amp$amp1= new ArgMetaInfo();
    $amp$amp1->name = "other";
    $amp$amp1->jvmName = NameTransformer::encode($amp$amp1->name);
    $amp$amp1->dataType = DataType::Kind::Bool;
    $amp$amp->args.push_back($amp$amp1);
    rec->methods[$amp$amp->name].push_back($amp$amp);

    MethodMetaInfo* orM = new MethodMetaInfo();
    orM->classMetaInfo = rec;
    orM->modifiers.modifiers.push_back(_PUBLIC);
    orM->returnType = DataType::Kind::Bool;
    orM->name = "or";
    orM->jvmName = NameTransformer::encode(orM->name);
    orM->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* orM1= new ArgMetaInfo();
    orM1->name = "other";
    orM1->jvmName = NameTransformer::encode(orM1->name);
    orM1->dataType = DataType::Kind::Bool;
    orM->args.push_back(orM1);
    rec->methods[orM->name].push_back(orM);

    MethodMetaInfo* $bar = new MethodMetaInfo();
    $bar->classMetaInfo = rec;
    $bar->modifiers.modifiers.push_back(_PUBLIC);
    $bar->returnType = DataType::Kind::Bool;
    $bar->name = "|";
    $bar->jvmName = NameTransformer::encode($bar->name);
    $bar->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $bar1= new ArgMetaInfo();
    $bar1->name = "other";
    $bar1->jvmName = NameTransformer::encode($bar1->name);
    $bar1->dataType = DataType::Kind::Bool;
    $bar->args.push_back($bar1);
    rec->methods[$bar->name].push_back($bar);

    MethodMetaInfo* $bar$bar = new MethodMetaInfo();
    $bar$bar->classMetaInfo = rec;
    $bar$bar->modifiers.modifiers.push_back(_PUBLIC);
    $bar$bar->returnType = DataType::Kind::Bool;
    $bar$bar->name = "||";
    $bar$bar->jvmName = NameTransformer::encode($bar$bar->name);
    $bar$bar->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $bar$bar1= new ArgMetaInfo();
    $bar$bar1->name = "other";
    $bar$bar1->jvmName = NameTransformer::encode($bar$bar1->name);
    $bar$bar1->dataType = DataType::Kind::Bool;
    $bar$bar->args.push_back($bar$bar1);
    rec->methods[$bar$bar->name].push_back($bar$bar);

    MethodMetaInfo* xorM = new MethodMetaInfo();
    xorM->classMetaInfo = rec;
    xorM->modifiers.modifiers.push_back(_PUBLIC);
    xorM->returnType = DataType::Kind::Bool;
    xorM->name = "xor";
    xorM->jvmName = NameTransformer::encode(xorM->name);
    xorM->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* xorM1= new ArgMetaInfo();
    xorM1->name = "other";
    xorM1->jvmName = NameTransformer::encode(xorM1->name);
    xorM1->dataType = DataType::Kind::Bool;
    xorM->args.push_back(xorM1);
    rec->methods[xorM->name].push_back(xorM);

    MethodMetaInfo* $up = new MethodMetaInfo();
    $up->classMetaInfo = rec;
    $up->modifiers.modifiers.push_back(_PUBLIC);
    $up->returnType = DataType::Kind::Bool;
    $up->name = "^";
    $up->jvmName = NameTransformer::encode($up->name);
    $up->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* $up1= new ArgMetaInfo();
    $up1->name = "other";
    $up1->jvmName = NameTransformer::encode($up1->name);
    $up1->dataType = DataType::Kind::Bool;
    $up->args.push_back($up1);
    rec->methods[$up->name].push_back($up);

    MethodMetaInfo* notM = new MethodMetaInfo();
    notM->classMetaInfo = rec;
    notM->modifiers.modifiers.push_back(_PUBLIC);
    notM->returnType = DataType::Kind::Bool;
    notM->name = "not";
    notM->jvmName = NameTransformer::encode(notM->name);
    notM->args = vector<ArgMetaInfo*>();
    rec->methods[notM->name].push_back(notM);

    MethodMetaInfo* $bang = new MethodMetaInfo();
    $bang->classMetaInfo = rec;
    $bang->modifiers.modifiers.push_back(_PUBLIC);
    $bang->returnType = DataType::Kind::Bool;
    $bang->name = "!";
    $bang->jvmName = NameTransformer::encode($bang->name);
    $bang->args = vector<ArgMetaInfo*>();
    rec->methods[$bang->name].push_back($bang);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg1= new ArgMetaInfo();
    equalsArg1->name = "o";
    equalsArg1->jvmName = NameTransformer::encode(equalsArg1->name);
    equalsArg1->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg1);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = rec;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->modifiers.modifiers.push_back(_OVERRIDE);
    toString->returnType = DataType::Kind::String;
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    MethodMetaInfo* toScalaString = new MethodMetaInfo();
    toScalaString->classMetaInfo = rec;
    toScalaString->modifiers.modifiers.push_back(_PUBLIC);
    toScalaString->modifiers.modifiers.push_back(_OVERRIDE);
    toScalaString->returnType = DataType::Kind::String;
    toScalaString->name = "toScalaString";
    toScalaString->jvmName = NameTransformer::encode(toScalaString->name);
    toScalaString->args = vector<ArgMetaInfo*>();
    rec->methods[toScalaString->name].push_back(toScalaString);

    // Унарный оператор отрицания
    MethodMetaInfo* unary_not = new MethodMetaInfo();
    unary_not->classMetaInfo = rec;
    unary_not->modifiers.modifiers.push_back(_PUBLIC);
    unary_not->returnType = DataType::Kind::Bool;
    unary_not->name = "unary_!";
    unary_not->jvmName = NameTransformer::encode(unary_not->name);
    unary_not->args = vector<ArgMetaInfo*>();
    rec->methods[unary_not->name].push_back(unary_not);

    return rec;
}

RtlClassMetaInfo *RtlClassMetaInfo::initPredef() {
    string className = "Predef";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _SCALA);
    rec->jvmName = "rtl/Predef";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor = new MethodMetaInfo();
    constructor->classMetaInfo = rec;
    constructor->modifiers.modifiers.push_back(_PUBLIC);
    constructor->returnType = DataType::Kind::Any;
    constructor->name = "this";
    constructor->jvmName = "<init>";
    constructor->args = vector<ArgMetaInfo*>();
    rec->methods[constructor->name].push_back(constructor);

    MethodMetaInfo* print = new MethodMetaInfo();
    print->classMetaInfo = rec;
    print->modifiers.modifiers.push_back(_PUBLIC);
    print->returnType = DataType::Kind::Unit;
    print->name = "print";
    print->jvmName = NameTransformer::encode(print->name);
    print->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* printArg = new ArgMetaInfo();
    printArg->name = "x";
    printArg->jvmName = NameTransformer::encode(printArg->name);
    printArg->dataType = DataType::Kind::Any;
    print->args.push_back(printArg);
    rec->methods[print->name].push_back(print);

    MethodMetaInfo* println0 = new MethodMetaInfo();
    println0->classMetaInfo = rec;
    println0->modifiers.modifiers.push_back(_PUBLIC);
    println0->returnType = DataType::Kind::Unit;
    println0->name = "println";
    println0->jvmName = NameTransformer::encode(println0->name);
    println0->args = vector<ArgMetaInfo*>();
    rec->methods[println0->name].push_back(println0);

    MethodMetaInfo* println1 = new MethodMetaInfo();
    println1->classMetaInfo = rec;
    println1->modifiers.modifiers.push_back(_PUBLIC);
    println1->returnType = DataType::Kind::Unit;
    println1->name = "println";
    println1->jvmName = NameTransformer::encode(println1->name);
    println1->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* printlnArg = new ArgMetaInfo();
    printlnArg->name = "x";
    printlnArg->jvmName = NameTransformer::encode(printlnArg->name);
    printlnArg->dataType = DataType::Kind::Class;
    printlnArg->dataType.className = "Any";
    println1->args.push_back(printlnArg);
    rec->methods[println1->name].push_back(println1);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
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
    equalsArg1->dataType.className = "Any";
    equals->args.push_back(equalsArg1);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    return rec;
}

RtlClassMetaInfo *RtlClassMetaInfo::initArray() {
    string className = "Array";

    RtlClassMetaInfo* rec = new RtlClassMetaInfo(className, _JAVA);
    rec->jvmName = "rtl/Array";

    rec->parent = Any;

    rec->modifiers.modifiers.push_back(_PUBLIC);
    rec->modifiers.modifiers.push_back(_FINAL);

    MethodMetaInfo* constructor1 = new MethodMetaInfo();
    constructor1->classMetaInfo = rec;
    constructor1->modifiers.modifiers.push_back(_PUBLIC);
    constructor1->returnType = DataType::Kind::Any;
    constructor1->name = "<init7>";
    constructor1->jvmName = NameTransformer::encode(constructor1->name);
    constructor1->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* constructor1Arg = new ArgMetaInfo();
    constructor1Arg->name = "length";
    constructor1Arg->jvmName = NameTransformer::encode(constructor1Arg->name);
    constructor1Arg->dataType = DataType::Kind::Int;
    constructor1->args.push_back(constructor1Arg);
    rec->methods[constructor1->name].push_back(constructor1);

    MethodMetaInfo* constructor2 = new MethodMetaInfo();
    constructor2->classMetaInfo = rec;
    constructor2->modifiers.modifiers.push_back(_PUBLIC);
    constructor2->returnType = DataType::Kind::Any;
    constructor2->name = "<init8>";
    constructor2->jvmName = NameTransformer::encode(constructor2->name);
    constructor2->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* constructor2Arg = new ArgMetaInfo();
    constructor2Arg->name = "length";
    constructor2Arg->jvmName = NameTransformer::encode(constructor2Arg->name);
    constructor2Arg->dataType = DataType::Kind::Array;
    DataType* arrayType = new DataType();
    arrayType->kind = DataType::Kind::Any;
    constructor2Arg->dataType.elementType = arrayType;
    constructor2->args.push_back(constructor2Arg);
    rec->methods[constructor2->name].push_back(constructor2);

    MethodMetaInfo* length = new MethodMetaInfo();
    length->classMetaInfo = rec;
    length->modifiers.modifiers.push_back(_PUBLIC);
    length->returnType = DataType::Kind::Int;
    length->name = "length";
    length->jvmName = NameTransformer::encode(length->name);
    length->args = vector<ArgMetaInfo*>();
    rec->methods[length->name].push_back(length);

    MethodMetaInfo* apply = new MethodMetaInfo();
    apply->classMetaInfo = rec;
    apply->modifiers.modifiers.push_back(_PUBLIC);
    apply->returnType = DataType::Kind::Any;
    apply->name = "apply";
    apply->jvmName = NameTransformer::encode(apply->name);
    apply->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* applyArg = new ArgMetaInfo();
    applyArg->name = "index";
    applyArg->jvmName = NameTransformer::encode(applyArg->name);
    applyArg->dataType = DataType::Kind::Int;
    apply->args.push_back(applyArg);
    rec->methods[apply->name].push_back(apply);

    MethodMetaInfo* update = new MethodMetaInfo();
    update->classMetaInfo = rec;
    update->modifiers.modifiers.push_back(_PUBLIC);
    update->returnType = DataType::Kind::Unit;
    update->name = "update";
    update->jvmName = NameTransformer::encode(update->name);
    update->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* updateArg1 = new ArgMetaInfo();
    updateArg1->name = "index";
    updateArg1->jvmName = NameTransformer::encode(updateArg1->name);
    updateArg1->dataType = DataType::Kind::Int;
    update->args.push_back(updateArg1);
    ArgMetaInfo* updateArg2 = new ArgMetaInfo();
    updateArg2->name = "value";
    updateArg2->jvmName = NameTransformer::encode(updateArg2->name);
    updateArg2->dataType = DataType::Kind::Any;
    update->args.push_back(updateArg2);
    rec->methods[update->name].push_back(update);

    MethodMetaInfo* toScalaString = new MethodMetaInfo();
    toScalaString->classMetaInfo = rec;
    toScalaString->modifiers.modifiers.push_back(_PUBLIC);
    toScalaString->modifiers.modifiers.push_back(_OVERRIDE);
    toScalaString->returnType = DataType::Kind::String;
    toScalaString->name = "toScalaString";
    toScalaString->jvmName = NameTransformer::encode(toScalaString->name);
    toScalaString->args = vector<ArgMetaInfo*>();
    rec->methods[toScalaString->name].push_back(toScalaString);

    MethodMetaInfo* toString = new MethodMetaInfo();
    toString->classMetaInfo = rec;
    toString->modifiers.modifiers.push_back(_PUBLIC);
    toString->modifiers.modifiers.push_back(_OVERRIDE);
    toString->returnType = DataType::Kind::String;
    toString->name = "toString";
    toString->jvmName = NameTransformer::encode(toString->name);
    toString->args = vector<ArgMetaInfo*>();
    rec->methods[toString->name].push_back(toString);

    MethodMetaInfo* equals = new MethodMetaInfo();
    equals->classMetaInfo = rec;
    equals->modifiers.modifiers.push_back(_PUBLIC);
    equals->modifiers.modifiers.push_back(_OVERRIDE);
    equals->returnType = DataType::Kind::Bool;
    equals->name = "equals";
    equals->jvmName = NameTransformer::encode(equals->name);
    equals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* equalsArg = new ArgMetaInfo();
    equalsArg->name = "other";
    equalsArg->jvmName = NameTransformer::encode(equalsArg->name);
    equalsArg->dataType = DataType::Kind::Any;
    equals->args.push_back(equalsArg);
    rec->methods[equals->name].push_back(equals);

    MethodMetaInfo* hashCode = new MethodMetaInfo();
    hashCode->classMetaInfo = rec;
    hashCode->modifiers.modifiers.push_back(_PUBLIC);
    hashCode->modifiers.modifiers.push_back(_OVERRIDE);
    hashCode->returnType = DataType::Kind::Int;
    hashCode->name = "hashCode";
    hashCode->jvmName = NameTransformer::encode(hashCode->name);
    hashCode->args = vector<ArgMetaInfo*>();
    rec->methods[hashCode->name].push_back(hashCode);

    MethodMetaInfo* notEquals = new MethodMetaInfo();
    notEquals->classMetaInfo = rec;
    notEquals->modifiers.modifiers.push_back(_PUBLIC);
    notEquals->modifiers.modifiers.push_back(_OVERRIDE);
    notEquals->returnType = DataType::Kind::Bool;
    notEquals->name = "notEquals";
    notEquals->jvmName = NameTransformer::encode(notEquals->name);
    notEquals->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* notEqualsArg = new ArgMetaInfo();
    notEqualsArg->name = "other";
    notEqualsArg->jvmName = NameTransformer::encode(notEqualsArg->name);
    notEqualsArg->dataType = DataType::Kind::Any;
    notEquals->args.push_back(notEqualsArg);
    rec->methods[notEquals->name].push_back(notEquals);

    MethodMetaInfo* is = new MethodMetaInfo();
    is->classMetaInfo = rec;
    is->modifiers.modifiers.push_back(_PUBLIC);
    is->modifiers.modifiers.push_back(_OVERRIDE);
    is->returnType = DataType::Kind::Bool;
    is->name = "is";
    is->jvmName = NameTransformer::encode(is->name);
    is->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* isArg = new ArgMetaInfo();
    isArg->name = "other";
    isArg->jvmName = NameTransformer::encode(isArg->name);
    isArg->dataType = DataType::Kind::Any;
    is->args.push_back(isArg);
    rec->methods[is->name].push_back(is);

    MethodMetaInfo* isNot = new MethodMetaInfo();
    isNot->classMetaInfo = rec;
    isNot->modifiers.modifiers.push_back(_PUBLIC);
    isNot->modifiers.modifiers.push_back(_OVERRIDE);
    isNot->returnType = DataType::Kind::Bool;
    isNot->name = "isNot";
    isNot->jvmName = NameTransformer::encode(isNot->name);
    isNot->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* isNotArg = new ArgMetaInfo();
    isNotArg->name = "other";
    isNotArg->jvmName = NameTransformer::encode(isNotArg->name);
    isNotArg->dataType = DataType::Kind::Any;
    isNot->args.push_back(isNotArg);
    rec->methods[isNot->name].push_back(isNot);

    MethodMetaInfo* isInstanceOf = new MethodMetaInfo();
    isInstanceOf->classMetaInfo = rec;
    isInstanceOf->modifiers.modifiers.push_back(_PUBLIC);
    isInstanceOf->modifiers.modifiers.push_back(_OVERRIDE);
    isInstanceOf->returnType = DataType::Kind::Bool;
    isInstanceOf->name = "isInstanceOf";
    isInstanceOf->jvmName = NameTransformer::encode(isInstanceOf->name);
    isInstanceOf->args = vector<ArgMetaInfo*>();
    ArgMetaInfo* isInstanceOfArg = new ArgMetaInfo();
    isInstanceOfArg->name = "cls";
    isInstanceOfArg->jvmName = NameTransformer::encode(isInstanceOfArg->name);
    isInstanceOfArg->dataType = DataType::Kind::Any;
    isInstanceOf->args.push_back(isInstanceOfArg);
    rec->methods[isInstanceOf->name].push_back(isInstanceOf);

    return rec;
}