//
// TypeCheckVisitor - проверяет типы выражений и присваиваний
// Должен вызываться ПОСЛЕ всех предыдущих фаз анализа
//

#ifndef SCALA_LEXER_TYPECHECKVISITOR_H
#define SCALA_LEXER_TYPECHECKVISITOR_H

#include <optional>
#include "nodes/Node.h"
#include "semantic/SemanticContext.h"
#include "semantic/scopes/Scope.h"
#include "semantic/tables/tables.hpp"
#include "semantic/tools/datatype.h"

class TopStatSeqNode;
class ClassDefNode;
class FunDefNode;
class BlockStatsNode;
class BlockStatNode;
class VarDefsNode;
class ExprNode;
class InfixExprNode;
class PrefixExprNode;
class SimpleExprNode;
class SimpleExpr1Node;
class AssignmentNode;

class TypeCheckVisitor {
private:
    ClassMetaInfo* currentClass = nullptr;
    MethodMetaInfo* currentMethod = nullptr;
    Scope* currentScope = nullptr;

    void visit(Node* node);
    void visitChildren(Node* node);

    void visitClassDef(ClassDefNode* node);
    void visitFunDef(FunDefNode* node);
    void visitBlockStats(BlockStatsNode* node);
    void visitBlockStat(BlockStatNode* node);
    void visitVarDefs(VarDefsNode* node);
    void visitExpr(ExprNode* node);
    void visitAssignment(AssignmentNode* node);
    void visitForExpression(ExprNode* node);

    // Проверка совместимости типов при присваивании
    void checkAssignmentCompatibility(const DataType& targetType, const DataType& sourceType,
                                      int line, const std::string& context);

    // Проверка что переменная не val при присваивании
    void checkNotVal(bool isVal, const std::string& varName, int line);

    void checkAssignment(VarMetaInfo *fieldInfo, const std::string &varName, const DataType &exprType, int line);

    static void validateConstructorExpr(ExprNode* expr, string constructorSignature);

public:
    TypeCheckVisitor() = default;
    void visitTree(TopStatSeqNode* root);
};

#endif //SCALA_LEXER_TYPECHECKVISITOR_H
