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
                                              argTypes.begin(),
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
    if (!varDefsNode || !varDefsNode->ids) return std::nullopt;

    DataType type = DataType::createFromNode(varDefsNode->simpleType);
    bool isVal = (varDefsNode->type == _VAL_DECL);

    for (auto *idNode: *varDefsNode->ids->ids) {
        string name = idNode->name;

        if (fields.count(name)) {
            ErrorTable::addErrorToList(new SemanticError(SemanticError::FieldRedefinition(0, name)));
            continue;
        }

        auto *field = new FieldMetaInfo();
        field->name = name;
        field->dataType = type;
        field->isVal = isVal;
        field->isInit = (varDefsNode->expr != nullptr);
        field->classMetaInfo = this;
        field->modifiers = modifiers;
        field->value = varDefsNode->expr; // Expression stored for later analysis

        this->fields[name] = field;
    }
    return std::nullopt;
}

optional<FieldMetaInfo *> ClassMetaInfo::addField(DclNode *varDclNode) {
    if (!varDclNode || !varDclNode->ids) return std::nullopt;

    DataType type = DataType::createFromNode(varDclNode->simpleType);
    bool isVal = (varDclNode->type == _VAL_DECL);
    Modifiers mods = Modifiers::createFromModifiersNode(*varDclNode->modifiers);

    for (auto *idNode: *varDclNode->ids->ids) {
        string name = idNode->name;

        if (fields.count(name)) {
            ErrorTable::addErrorToList(new SemanticError(SemanticError::FieldRedefinition(0, name)));
            continue;
        }

        auto *field = new FieldMetaInfo();
        field->name = name;
        field->dataType = type;
        field->isVal = isVal;
        field->classMetaInfo = this;
        field->modifiers = mods;
        field->isInit = false;

        this->fields[name] = field;
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

RtlClassMetaInfo* RtlClassMetaInfo::String = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Integer = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::SbtIn = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Unit = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Char = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Double = nullptr;
RtlClassMetaInfo* RtlClassMetaInfo::Boolean = nullptr;

static std::unordered_map<std::string, RtlClassMetaInfo*> rtlClassMap;

void RtlClassMetaInfo::initializeRtlClasses() {
    String = RtlClassMetaInfo::initString();
    Integer = RtlClassMetaInfo::initInteger();
    Double = RtlClassMetaInfo::initDouble();
    Boolean = RtlClassMetaInfo::initBoolean();
    Char = RtlClassMetaInfo::initChar();
    Unit = RtlClassMetaInfo::initUnit();
    SbtIn = RtlClassMetaInfo::initSbtIn();

    rtlClassMap["String"] = String;
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
    return new RtlClassMetaInfo("Boolean", _JAVA);
}