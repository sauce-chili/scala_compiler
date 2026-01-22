//
// Created by Dmitry Smirnov on 17.01.2026.
//

#include "ScopeAttachVisitor.h"

#include "nodes/func/FunDefNode.h"
#include "nodes/generator/EnumeratorsNode.h"
#include "nodes/templates/TemplateDefNode.h"

ScopeAttachVisitor::ScopeAttachVisitor() {
    currentScope = nullptr;
}

void ScopeAttachVisitor::visitTree(TopStatSeqNode *topStatSeq) {
    if (!topStatSeq) return;
    visit(reinterpret_cast<Node *>(topStatSeq));
}

void ScopeAttachVisitor::visit(Node *node) {
    if (!node) return;

    // 1. Метод -> Изоляция.
    // Мы просто обнуляем currentScope. Аргументы функции (FuncParams)
    // пока останутся "висеть в воздухе" (без скоупа),
    // так как они находятся ДО тела метода (BlockStats).
    // Но тело метода (BlockStats) корректно создаст свой корневой скоуп.
    if (auto *funDef = dynamic_cast<FunDefNode *>(node)) {
        handleFunDef(funDef);
    }
    // 2. Блок кода -> Создаем BLOCK Scope
    else if (auto *block = dynamic_cast<BlockStatsNode *>(node)) {
        handleBlockStats(block);
    }
    // 3. Цикл For -> Создаем FOR Scope
    // Проверяем наличие enumerators, так как это признак for-comprehension
    else if (auto *expr = dynamic_cast<ExprNode *>(node); expr && expr->enumerators) {
        handleForExpression(expr);
    }
    // 4. Все остальное -> Просто идем вглубь
    else {
        visitChildren(node);
    }
}

void ScopeAttachVisitor::visitChildren(Node *node) {
    for (Node *child: node->getChildren()) {
        visit(child);
    }
}

void ScopeAttachVisitor::handleBlockStats(BlockStatsNode *node) {
    // Создаем новый скоуп.
    // Если это начало метода, external будет nullptr.
    // Если вложенный блок, external будет указывать на родителя.
    Scope *blockScope = new Scope(currentScope, ScopeType::BLOCK);
    node->blockScope = blockScope;

    Scope *prev = currentScope;
    currentScope = blockScope;

    visitChildren(node);

    // Восстанавливаем скоуп
    currentScope = prev;
}

void ScopeAttachVisitor::handleForExpression(ExprNode *node) {
    Scope *forScope = new Scope(currentScope, ScopeType::FOR);

    // Сохраняем скоуп в EnumeratorsNode, как договаривались
    if (node->enumerators) {
        node->enumerators->scope = forScope;
    }

    Scope *prev = currentScope;
    currentScope = forScope;

    // Важно: благодаря getChildren, сначала посетим
    // node->enumerators (определим переменные в forScope),
    // а затем node->exprs (используем их в forScope).
    visitChildren(node);

    // Восстанавливаем скоуп
    currentScope = prev;
}

void ScopeAttachVisitor::handleFunDef(FunDefNode *node) {
    Scope *prev = currentScope;

    // "Сбрасываем" контекст, чтобы метод не видел внешние переменные
    currentScope = nullptr;

    currentScope = prev;

    // Посещаем остальные части (тело функции/конструктора)
    if (node->expr) visit(node->expr);
    if (node->constrExpr) visit(node->constrExpr);

    // Восстанавливаем скоуп
    currentScope = prev;
}
