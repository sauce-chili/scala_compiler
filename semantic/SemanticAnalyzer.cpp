//
// Created by Dmitry Smirnov on 21.01.2026.
//

#include "SemanticAnalyzer.h"

#include <fstream>
#include <algorithm>
#include <iostream>


#include "ClassMemberGatherVisitor.h"
#include "error/ErrorTable.h"
#include "tables/tables.hpp"

static std::string cleanMods(Modifiers m) {
    std::string s = m.toString();
    std::replace(s.begin(), s.end(), '\n', ' ');
    return s.empty() ? "public" : s;
}

void SemanticAnalyzer::exportClassesToCSV(const std::string& path) {
    std::ofstream clsFile(path + "/classes.csv");
    if (!clsFile.is_open()) return;

    clsFile << "Modifiers,ClassName,JvmName,ParentName,ConstantCount,MethodCount,FieldCount\n";

    for (auto const& [name, info] : ctx().classes) {
        if (!info) continue;

        // Считаем общее количество методов (включая все перегрузки)
        int methodCount = 0;
        for (auto const& [mName, mList] : info->methods) {
            methodCount += mList.size();
        }

        clsFile << "\"" << cleanMods(info->modifiers) << "\","
                << name << ","
                << info->jvmName << ","
                << (info->parent ? info->parent->name : "None") << ","
                << info->getConstantCounter() << "," // Счетчик констант из ClassMetaInfo
                << methodCount << ","
                << info->fields.size() << "\n";
    }
    clsFile.close();
}

void SemanticAnalyzer::exportMethodsToCSV(const std::string& path) {
    std::ofstream mthFile(path + "/methods.csv");
    if (!mthFile.is_open()) return;

    mthFile << "Modifiers,MethodName,JvmName,ArgTypes,ReturnType,ClassName,VarCount\n";

    for (auto const& [cName, cInfo] : ctx().classes) {
        for (auto const& [mName, mList] : cInfo->methods) {
            for (auto* m : mList) {
                // Формируем строку типов аргументов: (Int, String)
                std::string args = "(";
                for (size_t i = 0; i < m->args.size(); ++i) {
                    args += m->args[i]->dataType.toString();
                    if (i < m->args.size() - 1) args += ", ";
                }
                args += ")";

                std::string methodName = m->name;
                mthFile << "\"" << cleanMods(m->modifiers) << "\","
                        << methodName << ","
                        << m->jvmName << ','
                        << "\"" << args << "\","
                        << m->returnType.toString() << ","
                        << cName << ","
                        << m->localVarCounter << "\n"; // Количество переменных (number)
            }
        }
    }
    mthFile.close();
}

void SemanticAnalyzer::exportFieldsToCSV(const std::string& path) {
    std::ofstream fldFile(path + "/fields.csv");
    if (!fldFile.is_open()) return;

    fldFile << "Modifiers,FieldName,JvmName,Type,IsInitialized,ClassName\n";

    for (auto const& [cName, cInfo] : ctx().classes) {
        for (auto const& [fName, fInfo] : cInfo->fields) {
            fldFile << "\"" << cleanMods(fInfo->modifiers) << "\","
                    << fName << ","
                    << fInfo->jvmName << ","
                    << fInfo->dataType.toString() << ","
                    << (fInfo->isInit ? "true" : "false") << ","
                    << cName << "\n";
        }
    }
    fldFile.close();
}

void SemanticAnalyzer::exportContextToCSV(const std::string& path) {
    exportClassesToCSV(path);
    exportMethodsToCSV(path);
    exportFieldsToCSV(path);
}

static void checkFieldOverride(ClassMetaInfo* info, const std::string& name, FieldMetaInfo* field) {
    if (!info->parent) return;

    auto parentFieldOpt = info->parent->resolveField(name, info);

    if (parentFieldOpt.has_value()) {
        FieldMetaInfo* parentField = parentFieldOpt.value();

        // resolveField вернул поле, значит оно Visible (Public или Protected)
        // Проверяем правила Override

        if (parentField->isFinal()) {
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::FinalFieldOverride(0, name)
            ));
        }

        if (!field->isOverride()) {
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::FieldRedefinition(0, name + " (missing 'override' modifier)")
            ));
        }

        if (!(field->dataType == parentField->dataType)) {
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::TypeMismatch(0, parentField->dataType.toString(), field->dataType.toString())
            ));
        }

        if (parentField->isVal && !field->isVal) {
             ErrorTable::addErrorToList(new SemanticError(
                SemanticError::InvalidInheritance(0, "Cannot override val '" + name + "' with var")
            ));
        }
    } else {
        // Поле в родителе не найдено ИЛИ оно private.
        // Если мы написали 'override', это ошибка.
        if (field->isOverride()) {
            // Чтобы дать точное сообщение об ошибке, можно проверить existence через lookupPrivate=true
            // Но по стандарту Scala: "method overrides nothing" подходит и для случая с private.
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::UndefinedVar(0, "Field '" + name + "' overrides nothing (or parent field is private)")
            ));
        }
    }
}

static void checkMethodOverride(ClassMetaInfo* info, const std::string& name, MethodMetaInfo* method) {
    if (!info->parent) return;

    // Ищем метод, доступный наследнику
    auto parentMethodOpt = info->parent->resolveMethod(name, method->getArgsTypes(), info);

    if (parentMethodOpt.has_value()) {
        MethodMetaInfo* parentMethod = parentMethodOpt.value();

        if (parentMethod->isFinal()) {
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::FinalMethodOverride(0, name)
            ));
        }

        if (!method->isOverride()) {
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::MethodAlreadyExists(0, name + " (missing 'override' modifier)")
            ));
        }

        if (!(method->returnType == parentMethod->returnType)) {
             ErrorTable::addErrorToList(new SemanticError(
                SemanticError::ReturnTypeMismatch(0, parentMethod->returnType.toString(), method->returnType.toString())
            ));
        }
    } else {
        if (method->isOverride()) {
             ErrorTable::addErrorToList(new SemanticError(
                SemanticError::MethodNotFound(0, "Method '" + name + "' overrides nothing")
            ));
        }
    }
}

bool SemanticAnalyzer::analyze(TopStatSeqNode *root) {
    if (!root) return false;

    // Сбор деклараций
    ClassMemberGatherVisitor gatherVisitor;
    gatherVisitor.visitTree(root);

    if (!ErrorTable::errors.empty()) {
        std::cerr << "Errors after gather:" << std::endl << ErrorTable::getErrors() << std::endl;
        return false;
    }

    // связывание дочерних и родительских классов
    linkInheritanceHierarchy();
    // Проверка циклов наследования
    validateInheritanceCycles();
    // Валидация переопределений
    validateOverrides();

    if (!ErrorTable::errors.empty()) {
        std::cerr << "Semantic errors:" << std::endl << ErrorTable::getErrors() << std::endl;
        return false;
    }

    exportContextToCSV(".");
    return true;
}

void SemanticAnalyzer::linkInheritanceHierarchy() {
    for (auto& [name, info] : ctx().classes) {
        optional<std::string> parentNameOpt = info->getParentName();

        if (parentNameOpt.has_value()) {
            std::string parentName = parentNameOpt.value();
            if (ctx().classes.find(parentName) != ctx().classes.end()) {
                ClassMetaInfo* parentInfo = ctx().classes[parentName];

                // Проверка: наследование от final класса запрещено
                if (parentInfo->isFinal()) {
                    ErrorTable::addErrorToList(new SemanticError(
                        SemanticError::FinalClassInheritance(info->classNode->id, parentName)
                    ));
                }

                info->parent = parentInfo;
            } else {
                 ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::UndefinedClass(info->classNode->id, parentName)
                ));
            }
        }
    }
}

void SemanticAnalyzer::validateInheritanceCycles() {
    for (auto& [name, info] : ctx().classes) {
        ClassMetaInfo* curr = info->parent;
        // Простой алгоритм проверки циклов (можно оптимизировать через DFS/цвета,
        // но для дерева наследования достаточно прохода вверх)
        while (curr) {
            if (curr == info) {
                ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::InvalidInheritance(info->classNode->id, "Cycle detected for class " + name)
                ));
                // Разрываем цикл, чтобы не зависнуть
                info->parent = nullptr;
                break;
            }
            curr = curr->parent;
        }
    }
}

void SemanticAnalyzer::validateOverrides() {
    for (auto& [className, info] : ctx().classes) {
        // Поля
        for (auto& [fieldName, fieldInfo] : info->fields) {
            checkFieldOverride(info, fieldName, fieldInfo);
        }
        // Методы
        for (auto& [methodName, methodList] : info->methods) {
            for (auto* methodInfo : methodList) {
                checkMethodOverride(info, methodInfo->name, methodInfo);
            }
        }
    }
}

SemanticAnalyzer::SemanticAnalyzer() {
    RtlClassMetaInfo::initializeRtlClasses();
    ctx().addClass(RtlClassMetaInfo::Object);
    ctx().addClass(RtlClassMetaInfo::Any);
    ctx().addClass(RtlClassMetaInfo::String);
    ctx().addClass(RtlClassMetaInfo::Integer);
    ctx().addClass(RtlClassMetaInfo::StdIn);
    ctx().addClass(RtlClassMetaInfo::Predef);
    ctx().addClass(RtlClassMetaInfo::Char);
    ctx().addClass(RtlClassMetaInfo::Double);
    ctx().addClass(RtlClassMetaInfo::Boolean);
    ctx().addClass(RtlClassMetaInfo::Unit);
    ctx().addClass(RtlClassMetaInfo::Array);
}
