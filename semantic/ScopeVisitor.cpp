//
// Created by Dmitry Smirnov on 17.01.2026.
//

#include "ScopeVisitor.h"

#include "nodes/func/FuncParamsNode.h"
#include "nodes/func/FunDefNode.h"
#include "nodes/templates/TemplateDefNode.h"
#include "nodes/class/ClassDefNode.h"

ScopeVisitor::ScopeVisitor() {
    currentScope = new Scope(nullptr, ScopeType::GLOBAL);
}

void ScopeVisitor::visitTree(TopStatSeqNode *topStatSeq) {
    if (!topStatSeq) return;
    visit(reinterpret_cast<Node *>(topStatSeq));
}

void ScopeVisitor::visit(Node *node) {
    if (!node) return;

    // Проверяем узлы, меняющие Scope
    if (auto *tmpl = dynamic_cast<TemplateDefNode *>(node)) {
        handleClassScope(tmpl);
    } else if (auto *funcParams = dynamic_cast<FuncParamsNode *>(node)) {
        handleFuncParamsScope(funcParams);
    } else if (auto *funDef = dynamic_cast<FunDefNode *>(node)) {
        handleFunDef(funDef);
    } else if (auto *block = dynamic_cast<BlockStatsNode *>(node)) {
        handleBlockScope(block);
    }
    // для всех остальных узлов (If, While, Infix и тд)
    // просто рекурсивно вызываем visit для всех детей.
    else {
        visitChildren(node);
    }
}

void ScopeVisitor::visitChildren(Node *node) {
    for (Node *child: node->getChildren()) {
        visit(child);
    }
}

void ScopeVisitor::handleClassScope(TemplateDefNode *node) {
    std::string name = (node->classDef->fullId) ? node->classDef->fullId->name : "anon";

    Scope *classScope = new Scope(currentScope, name);
    node->classScope = classScope;

    Scope *prev = currentScope;
    currentScope = classScope;

    visitChildren(node);

    // Восстанавливаем скоуп
    currentScope = prev;
}

void ScopeVisitor::handleFuncParamsScope(FuncParamsNode *node) {
    // Создаем скоуп, но НЕ входим в него (currentScope не меняется, чтобы при декларациях не зависнуть в этом скоупе).
    // Он активируется позже, при обходе тела функции.
    Scope *argsScope = new Scope(currentScope, ScopeType::METHOD_ARGS);
    node->argsScope = argsScope;

    // Аргументы сами по себе просто декларируются здесь.
    visitChildren(node);
}

void ScopeVisitor::handleBlockScope(BlockStatsNode *node) {
    Scope *blockScope = new Scope(currentScope, ScopeType::BLOCK);
    node->blockScope = blockScope;

    Scope *prev = currentScope;
    currentScope = blockScope;

    visitChildren(node); // Все выражения блока обрабатываются в новом скоупе

    // Восстанавливаем скоуп
    currentScope = prev;
}

void ScopeVisitor::handleFunDef(FunDefNode *node) {
    // 1. Посещаем сигнатуру (там внутри вызовется handleFuncParamsScope и создастся scope)
    if (node->funSig) {
        visit(node->funSig);
    }

    // 2. Обработка тела функции
    // Нам нужно найти созданный скоуп аргументов
    Scope* methodScope = nullptr;
    if (node->funSig && node->funSig->params) {
        methodScope = node->funSig->params->argsScope;
    }

    // Если скоуп нашли - используем его как текущий для тела
    Scope* prev = currentScope;
    if (methodScope) {
        currentScope = methodScope;
    }

    // 3. Посещаем остальные части (тело функции/конструктора)
    if (node->expr) visit(node->expr);
    if (node->constrExpr) visit(node->constrExpr);

    // Восстанавливаем скоуп
    currentScope = prev;
}

