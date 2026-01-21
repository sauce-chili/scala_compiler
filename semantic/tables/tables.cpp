//
// Created by Dmitry Smirnov on 17.01.2026.
//

#include "tables.hpp"

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

optional<FieldMetaInfo *> ClassMetaInfo::resolveField(string fieldName) {
    if (fields.find(fieldName) == fields.end()) {
        return nullopt;
    }
    return optional{fields.at(fieldName)};
}

optional<MethodMetaInfo *> ClassMetaInfo::resolveMethod(string methodName, vector<DataType *> argTypes) {
    if (methods.find(methodName) == methods.end()) {
        return nullopt;
    }

    vector<MethodMetaInfo *> methodsSignature = methods.at(methodName);

    for (auto const m: methodsSignature) {
        vector<DataType *> existingArgs = m->getArgsTypes();

        bool countArgsEquals = existingArgs.size() == argTypes.size();
        bool typesArgsEquals = std::equal(existingArgs.begin(), existingArgs.end(),
                                          argTypes.begin(),
                                          [](const DataType *a, const DataType *b) {
                                              return *a == *b;
                                          });

        bool signatureEquals = typesArgsEquals && countArgsEquals;

        if (signatureEquals) {
            return optional{m};
        }
    }

    // TODO после связавание предка с родителем дописать обращение к родителю

    return nullopt;
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
    vector<ArgMetaInfo*> tempArgs;
    vector<DataType*> signatureTypes; // Указатели на типы для проверки

    int tempCounter = 0;
    if (funDefNode->funSig->params && funDefNode->funSig->params->funcParams) {
        for (auto *paramNode: *funDefNode->funSig->params->funcParams) {
             ArgMetaInfo* arg = new ArgMetaInfo();
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
    if (this->resolveMethod(methodName, signatureTypes).has_value()) {
        string sigStr = methodName + "(";
        for(size_t i = 0; i < signatureTypes.size(); ++i) {
            sigStr += signatureTypes[i]->toString();
            if (i < signatureTypes.size() - 1) sigStr += ", ";
        }
        sigStr += ")";

        ErrorTable::addErrorToList(new SemanticError(SemanticError::MethodAlreadyExists(0, sigStr)));

        // Очищаем память временных аргументов, так как метод не создан
        for(auto* a : tempArgs) delete a;
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
    for(auto* arg : tempArgs) {
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
    vector<ArgMetaInfo*> tempArgs;
    vector<DataType*> signatureTypes;
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
    if (this->resolveMethod(methodName, signatureTypes).has_value()) {
        string sigStr = methodName + "(...)";
        ErrorTable::addErrorToList(new SemanticError(SemanticError::MethodAlreadyExists(funDclNode->funSig->fullId->id, sigStr)));

        for(auto* a : tempArgs) delete a;
        return std::nullopt;
    }

    // Регистрация метода
    MethodMetaInfo *method = new MethodMetaInfo();
    method->name = methodName;
    method->classMetaInfo = this;
    method->returnType = DataType::createFromNode(funDclNode->simpleType);
    method->modifiers = Modifiers::createFromModifiersNode(*funDclNode->modifiers);
    method->localVarCounter = tempCounter;

    for(auto* arg : tempArgs) {
        arg->methodMetaInfo = method;
        method->args.push_back(arg);
    }

    this->methods[methodName].push_back(method);
    return method;
}
