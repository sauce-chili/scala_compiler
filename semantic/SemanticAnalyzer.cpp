//
// Created by Dmitry Smirnov on 21.01.2026.
//

#include "SemanticAnalyzer.h"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <filesystem>

#include "ClassMemberGatherVisitor.h"
#include "ScopeAttachVisitor.h"
#include "LocalVarGatherVisitor.h"
#include "TypeExistenceValidator.h"
#include "TypeCheckVisitor.h"
#include "constants/ConstantPoolVisitor.h"
#include "SemanticContext.h"
#include "error/ErrorTable.h"
#include "tables/tables.hpp"
#include "utils/output/dot.cpp"
#include "utils/output/tree_converter.cpp"

namespace fs = std::filesystem;

static std::string cleanMods(Modifiers m) {
    std::string s = m.toString();
    std::replace(s.begin(), s.end(), '\n', ' ');
    return s.empty() ? "public" : s;
}

static std::string scopeTypeToString(ScopeType type) {
    switch (type) {
        case ScopeType::BLOCK: return "BLOCK";
        case ScopeType::FOR: return "FOR";
        default: return "UNKNOWN";
    }
}

static std::string makeMethodFileName(MethodMetaInfo* m) {
    std::string fileName = m->name;
    for (auto* arg : m->args) {
        fileName += "_" + arg->dataType.toString();
    }
    std::replace(fileName.begin(), fileName.end(), '[', '_');
    std::replace(fileName.begin(), fileName.end(), ']', '_');
    std::replace(fileName.begin(), fileName.end(), ',', '_');
    std::replace(fileName.begin(), fileName.end(), ' ', '_');
    return fileName + ".csv";
}

static void prepareExportDir(const std::string& path) {
    fs::path exportPath(path);
    if (fs::exists(exportPath)) {
        fs::remove_all(exportPath);
    }
    fs::create_directories(exportPath);
}

static void exportClassInfo(const std::string& classDir, ClassMetaInfo* info) {
    std::ofstream file(classDir + "/class_info.csv");
    if (!file.is_open()) return;

    int methodCount = 0;
    for (auto const& [mName, mList] : info->methods) {
        methodCount += static_cast<int>(mList.size());
    }

    file << "Modifiers,Name,JvmName,Parent,FieldCount,MethodCount,ConstantCount\n";
    file << "\"" << cleanMods(info->modifiers) << "\","
         << info->name << ","
         << info->jvmName << ","
         << (info->parent ? info->parent->name : "-") << ","
         << info->fields.size() << ","
         << methodCount << ","
         << info->getConstantCounter() << "\n";
    file.close();
}

static void exportClassFields(const std::string& classDir, ClassMetaInfo* info) {
    std::ofstream file(classDir + "/fields.csv");
    if (!file.is_open()) return;

    file << "Modifiers,ValVar,Name,JvmName,Type,IsInit,HasValue\n";

    for (auto const& [fName, fInfo] : info->fields) {
        file << "\"" << cleanMods(fInfo->modifiers) << "\","
             << (fInfo->isVal ? "val" : "var") << ","
             << fName << ","
             << fInfo->jvmName << ","
             << fInfo->dataType.toString() << ","
             << (fInfo->isInit ? "true" : "false") << ","
             << (fInfo->value ? "true" : "false") << "\n";
    }
    file.close();
}

static void exportClassMethods(const std::string& classDir, ClassMetaInfo* info) {
    std::ofstream file(classDir + "/methods.csv");
    if (!file.is_open()) return;

    file << "Modifiers,MethodName,JvmName,ArgTypes,ReturnType,LocalVarCount\n";

    for (auto const& [mName, mList] : info->methods) {
        for (auto* m : mList) {
            std::string args = "(";
            for (size_t i = 0; i < m->args.size(); ++i) {
                args += m->args[i]->dataType.toString();
                if (i < m->args.size() - 1) args += ", ";
            }
            args += ")";

            file << "\"" << cleanMods(m->modifiers) << "\","
                 << mName << ","
                 << m->jvmName << ','
                 << "\"" << args << "\","
                 << m->returnType.toString() << ","
                 << m->localVarCounter << "\n";
        }
    }
    file.close();
}

static void exportMethodLocalVars(const std::string& localVarsDir, MethodMetaInfo* method) {
    std::string fileName = localVarsDir + "/" + makeMethodFileName(method);
    std::ofstream file(fileName);
    if (!file.is_open()) return;

    file << "Category,ValVar,Name,JvmName,Type,IsInit,HasValue,Number,ScopeId,ScopeType\n";

    // Сначала аргументы
    for (auto* arg : method->args) {
        file << "ARG,"
             << (arg->isVal ? "val" : "var") << ","
             << arg->name << ","
             << arg->jvmName << ","
             << arg->dataType.toString() << ","
             << (arg->isInit ? "true" : "false") << ","
             << (arg->value ? "true" : "false") << ","
             << arg->number << ","
             << "-,-\n";
    }

    // Локальные переменные (сортируем по number)
    std::vector<LocalVarMetaInfo*> allLocals;
    for (auto& [varName, scopeMap] : method->localVars) {
        for (auto& [scopeId, localVar] : scopeMap) {
            allLocals.push_back(localVar);
        }
    }

    std::sort(allLocals.begin(), allLocals.end(),
              [](LocalVarMetaInfo* a, LocalVarMetaInfo* b) { return a->number < b->number; });

    for (auto* local : allLocals) {
        file << "LOCAL,"
             << (local->isVal ? "val" : "var") << ","
             << local->name << ","
             << local->jvmName << ","
             << local->dataType.toString() << ","
             << (local->isInit ? "true" : "false") << ","
             << (local->value ? "true" : "false") << ","
             << local->number << ","
             << (local->scope ? std::to_string(local->scope->scopeId) : "-") << ","
             << (local->scope ? scopeTypeToString(local->scope->type) : "-") << "\n";
    }
    file.close();
}

static void exportConstantPool(const std::string& classDir, ClassMetaInfo* info) {
    if (!info || !info->constantPool) return;

    std::ofstream file(classDir + "/constants.csv");
    if (!file.is_open()) return;

    file << info->constantPool->toCsv();
    file.close();
}

void SemanticAnalyzer::exportContext(const std::string& basePath) {
    std::string exportDir = basePath + "/out_export";
    prepareExportDir(exportDir);

    for (auto const& [className, classInfo] : ctx().classes) {
        if (!classInfo) continue;
        // Пропускаем RTL классы
        if (classInfo->isRTL()) continue;

        // Создаём папку для класса
        std::string classDir = exportDir + "/" + className;
        fs::create_directories(classDir);

        // Экспортируем информацию о классе
        exportClassInfo(classDir, classInfo);

        // Экспортируем поля
        exportClassFields(classDir, classInfo);

        // Экспортируем методы
        exportClassMethods(classDir, classInfo);

        // Создаём папку для локальных переменных
        std::string localVarsDir = classDir + "/local_vars";
        fs::create_directories(localVarsDir);

        // Экспортируем локальные переменные для каждого метода
        for (auto const& [mName, mList] : classInfo->methods) {
            for (auto* method : mList) {
                exportMethodLocalVars(localVarsDir, method);
            }
        }

        // Экспортируем таблицу констант
        exportConstantPool(classDir, classInfo);
    }
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

    // Инициализация RTL классов
    ctx().ensureRtlInitialized();

    // Сбор деклараций классов, методов, полей
    ClassMemberGatherVisitor gatherVisitor;
    gatherVisitor.visitTree(root);

    if (!ErrorTable::errors.empty()) {
        std::cerr << "Errors after gather:" << std::endl << ErrorTable::getErrors() << std::endl;
        return false;
    }

    // Расстановка скоупов в AST
    ScopeAttachVisitor scopeVisitor;
    scopeVisitor.visitTree(root);

    createDotTree(root, "after_scope_attach.txt");

    // Сбор локальных переменных
    LocalVarGatherVisitor localVarVisitor;
    localVarVisitor.visitTree(root);

    if (!ErrorTable::errors.empty()) {
        std::cerr << "Errors after local var gather:" << std::endl << ErrorTable::getErrors() << std::endl;
        return false;
    }

    // Связывание дочерних и родительских классов
    linkInheritanceHierarchy();

    // Проверка существования типов
    TypeExistenceValidator typeValidator;
    typeValidator.visitTree(root);

    if (!ErrorTable::errors.empty()) {
        std::cerr << "Errors after type existence validation:" << std::endl << ErrorTable::getErrors() << std::endl;
        return false;
    }

    // Проверка циклов наследования
    validateInheritanceCycles();
    // Валидация переопределений (override/final)
    validateOverrides();
    // Проверяем что абстрактные методы реализованы
    validateAbstractMethods();
    // Проверяем что абстрактные поля переопределены
    validateAbstractFields();

    // 9. Проверяем наличие main метода
    // validateMainMethod();

    if (!ErrorTable::errors.empty()) {
        std::cerr << "Semantic errors after validation:" << std::endl << ErrorTable::getErrors() << std::endl;
        return false;
    }

    // Проверка типов и вызовов методов
    TypeCheckVisitor typeChecker;
    typeChecker.visitTree(root);

    if (!ErrorTable::errors.empty()) {
        std::cerr << "Semantic errors after type check:" << std::endl << ErrorTable::getErrors() << std::endl;
        return false;
    }

    // Сбор констант для кодогенерации
    ConstantPoolVisitor constantPoolVisitor;
    constantPoolVisitor.visitTree(root);

    exportContext(".");
    createScalaCode(root, "./generated.scala");
    return true;
}

void SemanticAnalyzer::linkInheritanceHierarchy() {
    for (auto& [name, info] : ctx().classes) {
        // Пропускаем RTL классы
        if (info->isRTL()) continue;

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
        // Пропускаем RTL классы
        if (info->isRTL()) continue;

        ClassMetaInfo* curr = info->parent;
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
        // Пропускаем RTL классы
        if (info->isRTL()) continue;

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
    // RTL классы инициализируются через ctx().ensureRtlInitialized() в analyze()
}

void SemanticAnalyzer::validateAbstractFields() {
    for (auto& [className, classInfo] : ctx().classes) {
        // Пропускаем RTL классы
        if (classInfo->isRTL()) continue;

        bool isAbstractClass = classInfo->modifiers.hasModifier(_ABSTRACT);

        // Проверка унаследованных абстрактных полей (только для конкретных классов)
        if (!isAbstractClass && classInfo->parent) {
            ClassMetaInfo* currParent = classInfo->parent;

            while (currParent) {
                for (auto& [pFieldName, pField] : currParent->fields) {
                    // Абстрактное поле - это поле без инициализации в абстрактном классе
                    if (!pField->isInit && currParent->modifiers.hasModifier(_ABSTRACT)) {
                        // Проверяем что поле переопределено в текущем классе
                        auto fieldOpt = classInfo->fields.find(pFieldName);
                        if (fieldOpt == classInfo->fields.end()) {
                            ErrorTable::addErrorToList(new SemanticError(
                                SemanticError::AbstractFieldNotOverride(
                                    classInfo->classNode->id,
                                    "Field '" + pFieldName + "' is abstract in '" + currParent->name + "' but not overridden in '" + className + "'"
                                )
                            ));
                        } else if (!fieldOpt->second->isInit) {
                            ErrorTable::addErrorToList(new SemanticError(
                                SemanticError::AbstractFieldNotOverride(
                                    classInfo->classNode->id,
                                    "Field '" + pFieldName + "' must be initialized in '" + className + "'"
                                )
                            ));
                        }
                    }
                }
                currParent = currParent->parent;
            }
        }
    }
}

// TODO возврщать список классов-кандидатов
void SemanticAnalyzer::validateMainMethod() {
    int mainCount = 0;
    ClassMetaInfo* mainClass = nullptr;

    for (auto& [className, classInfo] : ctx().classes) {
        // Пропускаем RTL классы
        if (classInfo->isRTL()) continue;

        // main должен быть в final классе без параметров конструктора
        if (!classInfo->isFinal()) continue;

        // Ищем метод main(Array[String]): Unit
        auto mainIt = classInfo->methods.find("main");
        if (mainIt != classInfo->methods.end()) {
            for (auto* method : mainIt->second) {
                // Проверяем сигнатуру: один аргумент Array[String], возврат Unit
                if (method->args.size() == 1) {
                    DataType& argType = method->args[0]->dataType;
                    if (argType.kind == DataType::Kind::Array) {
                        DataType* elemType = argType.elementType;
                        if (elemType && elemType->kind == DataType::Kind::String) {
                            if (method->returnType.kind == DataType::Kind::Unit) {
                                mainCount++;
                                mainClass = classInfo;
                            }
                        }
                    }
                }
            }
        }
    }

    if (mainCount == 0) {
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::MainMethodNotDefined(0)
        ));
    } else if (mainCount > 1) {
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::MethodAlreadyExists(0, "Multiple main methods found")
        ));
    }
}

void SemanticAnalyzer::validateAbstractMethods() {
    for (auto& [className, classInfo] : ctx().classes) {
        // Пропускаем RTL классы
        if (classInfo->isRTL()) continue;

        bool isAbstractClass = classInfo->modifiers.hasModifier(_ABSTRACT);

        // 1. Проверка локальных методов текущего класса
        for (auto& [methodName, methodList] : classInfo->methods) {
            for (auto* method : methodList) {
                // Если у метода нет тела (nullptr), он абстрактный.
                if (method->body == nullptr) {
                    if (!isAbstractClass) {
                        // Ошибка: Класс должен быть абстрактным
                        ErrorTable::addErrorToList(new SemanticError(
                            SemanticError::AbstractMethodNotImplemented(
                                classInfo->classNode->id,
                                "Class '" + className + "' must be declared abstract because it declares abstract method '" + methodName + "'"
                            )
                        ));
                    }
                }
            }
        }

        // 2. Проверка унаследованных методов (только для КОНКРЕТНЫХ классов)
        // Если класс абстрактный, он имеет право не реализовывать методы родителя.
        if (!isAbstractClass && classInfo->parent) {
            ClassMetaInfo* currParent = classInfo->parent;

            // Поднимаемся по иерархии вверх
            while (currParent) {
                for (auto& [pMethodName, pMethodList] : currParent->methods) {
                    for (auto* pMethod : pMethodList) {
                        // Нашли абстрактный метод у родителя
                        if (pMethod->body == nullptr) {
                            // Пытаемся найти реализацию этого метода в нашем классе (или в промежуточных родителях)
                            // resolveMethod ищет снизу вверх (child -> parent -> grandParent)
                            auto implementationOpt = classInfo->resolveMethod(pMethodName, pMethod->getArgsTypes(), classInfo);

                            bool implemented = false;
                            if (implementationOpt.has_value()) {
                                MethodMetaInfo* implementation = implementationOpt.value();
                                // Если найденный метод имеет тело, значит он реализован
                                if (implementation->body != nullptr) {
                                    implemented = true;
                                }
                            }

                            if (!implemented) {
                                ErrorTable::addErrorToList(new SemanticError(
                                    SemanticError::AbstractMethodNotImplemented(
                                        classInfo->classNode->id,
                                        "Class '" + className + "' must be declared abstract because it does not implement abstract member '" + pMethodName + "' from class '" + currParent->name + "'"
                                    )
                                ));
                            }
                        }
                    }
                }
                currParent = currParent->parent;
            }
        }
    }
}
