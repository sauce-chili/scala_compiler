#ifndef SCALA_LEXER_CONSTANTPOOLVISITOR_H
#define SCALA_LEXER_CONSTANTPOOLVISITOR_H

#include "ConstantPoolBuilder.h"

class TopStatSeqNode;
class ClassDefNode;
class FunDefNode;
class ExprNode;
class InfixExprNode;
class PrefixExprNode;
class SimpleExprNode;
class SimpleExpr1Node;
class BlockStatsNode;
class BlockStatNode;
class VarDefsNode;
class AssignmentNode;
class Node;

class ClassMetaInfo;
class MethodMetaInfo;
class FieldMetaInfo;
class Scope;

/**
 * Визитор для сбора констант из AST.
 * Проходит по дереву и добавляет все литералы, ссылки на методы и поля
 * в ConstantPoolBuilder текущего класса.
 */
class ConstantPoolVisitor {
private:
    ClassMetaInfo* currentClass = nullptr;
    MethodMetaInfo* currentMethod = nullptr;
    Scope* currentScope = nullptr;

public:
    /**
     * Точка входа: обход всего дерева.
     */
    void visitTree(TopStatSeqNode* root);

private:
    void visit(Node* node);
    void visitChildren(Node* node);

    void visitClassDef(ClassDefNode* node);
    void visitFunDef(FunDefNode* node);
    void visitBlockStats(BlockStatsNode* node);
    void visitBlockStat(BlockStatNode* node);
    void visitVarDefs(VarDefsNode* node);
    void visitExpr(ExprNode* node);
    void visitInfixExpr(InfixExprNode* node);
    void visitPrefixExpr(PrefixExprNode* node);
    void visitSimpleExpr(SimpleExprNode* node);
    void visitSimpleExpr1(SimpleExpr1Node* node);
    void visitAssignment(AssignmentNode* node);

    // Сбор внешних ссылок
    void collectMethodCallRefs(SimpleExpr1Node* node);
    void collectFieldAccessRefs(SimpleExpr1Node* node);

    // Вспомогательные методы для добавления констант
    void addLiteralConstant(SimpleExpr1Node* literal);
    void addMethodRefConstant(ClassMetaInfo* targetClass, MethodMetaInfo* method);
    void addFieldRefConstant(ClassMetaInfo* targetClass, FieldMetaInfo* field);
};

#endif // SCALA_LEXER_CONSTANTPOOLVISITOR_H
