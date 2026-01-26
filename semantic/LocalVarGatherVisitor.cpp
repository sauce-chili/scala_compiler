//
// LocalVarGatherVisitor - собирает локальные переменные в таблицы методов
//

#include "LocalVarGatherVisitor.h"

#include "nodes/nodes.h"
#include "nodes/func/FunDefNode.h"
#include "nodes/class/ClassDefNode.h"
#include "nodes/stats/BlockStatsNode.h"
#include "nodes/stats/BlockStatNode.h"
#include "nodes/var/VarDefsNode.h"
#include "nodes/generator/EnumeratorsNode.h"
#include "nodes/generator/GeneratorNode.h"
#include "nodes/exprs/ExprNode.h"
#include "semantic/tables/tables.hpp"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"

void LocalVarGatherVisitor::visitTree(TopStatSeqNode* root) {
    if (root) visit(root);
}

void LocalVarGatherVisitor::visit(Node* node) {
    if (!node) return;

    // 1. Класс
    if (auto* classDef = dynamic_cast<ClassDefNode*>(node)) {
        visitClassDef(classDef);
        return;
    }

    // 2. Метод (внутри класса)
    if (currentClass) {
        if (auto* funDef = dynamic_cast<FunDefNode*>(node)) {
            visitFunDef(funDef);
            return;
        }
    }

    // 3. Блок кода (внутри метода)
    if (currentMethod) {
        if (auto* blockStats = dynamic_cast<BlockStatsNode*>(node)) {
            visitBlockStats(blockStats);
            return;
        }

        // 4. For-выражение с генераторами
        if (auto* expr = dynamic_cast<ExprNode*>(node); expr && expr->enumerators) {
            visitForExpression(expr);
            return;
        }
    }

    // По умолчанию: идем вглубь
    visitChildren(node);
}

void LocalVarGatherVisitor::visitChildren(Node* node) {
    for (Node* child : node->getChildren()) {
        visit(child);
    }
}

void LocalVarGatherVisitor::visitClassDef(ClassDefNode* node) {
    if (!node->fullId) return;

    std::string className = node->fullId->name;
    auto it = ctx().classes.find(className);
    if (it == ctx().classes.end()) {
        throw std::logic_error("Не найден класс, который должен быть заполнен в ClassMemberGatherVisitor");
    }

    ClassMetaInfo* prevClass = currentClass;
    currentClass = it->second;

    // Обходим тело класса
    if (node->classTemplateOpt) {
        visit(node->classTemplateOpt);
    }

    currentClass = prevClass;
}

void LocalVarGatherVisitor::visitFunDef(FunDefNode* node) {
    if (!currentClass) return;

    // Находим метод в таблице текущего класса
    std::string methodName;
    std::vector<DataType*> argTypes;

    if (node->funSig && node->funSig->fullId) {
        methodName = node->funSig->fullId->name;

        // Собираем типы аргументов для поиска по сигнатуре
        if (node->funSig->params && node->funSig->params->funcParams) {
            for (auto* paramNode : *node->funSig->params->funcParams) {
                DataType* argType = new DataType(DataType::createFromNode(paramNode->simpleType));
                argTypes.push_back(argType);
            }
        }
    } else if (node->isConstructor() && node->funcParams) {
        // Вторичный конструктор // TODO или как this?
        methodName = currentClass->name;
        if (node->funcParams->funcParams) {
            for (auto* paramNode : *node->funcParams->funcParams) {
                DataType* argType = new DataType(DataType::createFromNode(paramNode->simpleType));
                argTypes.push_back(argType);
            }
        }
    } else {
        throw logic_error("Некорретное определение функции");
    }

    // передаём currentClass как accessFrom для доступа к приватным методам
    auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);

    for (const auto* t : argTypes) delete t;

    if (!methodOpt.has_value()) {
        throw std::logic_error("Не найден метод, который должен быть заполнен в ClassMemberGatherVisitor");
    }

    MethodMetaInfo* prevMethod = currentMethod;
    Scope* prevScope = currentScope;

    currentMethod = methodOpt.value();
    currentScope = nullptr; // Сбрасываем, будет установлен в BlockStats

    // Посещаем тело метода
    if (node->expr) visit(node->expr);
    if (node->constrExpr) visit(node->constrExpr);

    currentMethod = prevMethod;
    currentScope = prevScope;
}

void LocalVarGatherVisitor::visitBlockStats(BlockStatsNode* node) {
    if (!currentMethod) return;

    // Сохраняем предыдущий скоуп
    Scope* prevScope = currentScope;

    // Используем скоуп, расставленный ScopeAttachVisitor
    currentScope = node->blockScope;

    // Обходим все statement'ы в блоке
    if (node->blockStats) {
        for (auto* blockStat : *node->blockStats) {
            visitBlockStat(blockStat);
        }
    }

    // Восстанавливаем скоуп
    currentScope = prevScope;
}

void LocalVarGatherVisitor::visitBlockStat(BlockStatNode* node) {
    if (!node) return;

    // Если это объявление переменной
    if (node->varDefs) {
        visitVarDefs(node->varDefs, currentScope);
    }

    // Также обходим выражение (там могут быть вложенные блоки или for)
    if (node->expr) {
        visit(node->expr);
    }
}

void LocalVarGatherVisitor::visitVarDefs(VarDefsNode* node, Scope* scope) {
    if (!node || !node->fullId || !currentMethod || !scope) return;

    int line = node->fullId->id;
    std::string varName = node->fullId->name;

    // Используем метод MethodMetaInfo::addLocalVar
    auto result = currentMethod->addLocalVar(node, scope);

    if (!result.has_value()) {
        // Переменная уже определена в этом скоупе
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::VarRedefinition(line, varName)
        ));
    }
}

void LocalVarGatherVisitor::visitForExpression(ExprNode* node) {
    if (!node || !node->enumerators || !currentMethod) return;

    // Сохраняем предыдущий скоуп
    Scope* prevScope = currentScope;

    // Используем скоуп for-выражения
    currentScope = node->enumerators->scope;

    // Обрабатываем генератор (переменную цикла)
    if (node->enumerators->generator) {
        visitGenerator(node->enumerators->generator, currentScope);
    }

    // Обрабатываем тело цикла (exprs)
    if (node->exprs) {
        for (auto* expr : *node->exprs) {
            visit(expr);
        }
    }

    // Восстанавливаем скоуп
    currentScope = prevScope;
}

void LocalVarGatherVisitor::visitGenerator(GeneratorNode* node, Scope* scope) {
    if (!node || !node->fullId || !currentMethod || !scope) return;

    auto result = currentMethod->addGeneratorVar(node, scope);
    if (!result.has_value()) {
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::VarRedefinition(0, node->fullId->name)
        ));
    }
}

