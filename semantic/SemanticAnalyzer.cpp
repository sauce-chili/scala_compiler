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

bool SemanticAnalyzer::analyze(TopStatSeqNode *root) {
    if (!root) return false;

    // 1. Первичный сбор деклараций классов, полей и методов
    ClassMemberGatherVisitor gatherVisitor;
    gatherVisitor.visitTree(root);

    // Если есть ошибки (например, переопределение классов), выводим их
    if (!ErrorTable::errors.empty()) {
        std::cerr << "Semantic errors found:" << std::endl;
        std::cerr << ErrorTable::getErrors() << std::endl;
        return false;
    }

    // Экспорт таблицы методов для отладки
    exportMethodsToCSV("methods_dump.csv");

    return true;
}

void SemanticAnalyzer::exportMethodsToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
        return;
    }

    // Модификаторы, Название метода, Типы аргументов, Возвращаемое значение, Имя класса, Кол-во переменных
    file << "Modifiers,MethodName,ArgTypes,ReturnType,ClassName,VarCount\n";

    auto& classes = ctx().classes;

    for (auto const& [className, classInfo] : classes) {
        if (!classInfo) continue;

        // Проходим по всем группам методов (имя -> список перегрузок)
        for (auto const& [methodName, methodList] : classInfo->methods) {
            // Проходим по конкретным перегрузкам метода
            for (auto* m : methodList) {

                // модификаторы
                std::string mods = m->modifiers.toString();
                std::replace(mods.begin(), mods.end(), '\n', ' ');
                if (mods.empty()) mods = "public"; // По умолчанию public, если пусто

                // аргументы
                std::string argsStr = "(";
                for (size_t i = 0; i < m->args.size(); ++i) {
                    argsStr += m->args[i]->dataType.toString();
                    if (i < m->args.size() - 1) argsStr += ", ";
                }
                argsStr += ")";

                // возвращаемое значение
                std::string retType = m->returnType.toString();

                // кол-во переменных (аргументы + локальные)
                int varCount = m->localVarCounter;

                file << "\"" << mods << "\","
                     << methodName << ","
                     << "\"" << argsStr << "\","
                     << retType << ","
                     << className << ","
                     << varCount << "\n";
            }
        }
    }

    file.close();
    std::cout << "Таблица методов экспортирована в " << filename << std::endl;
}

