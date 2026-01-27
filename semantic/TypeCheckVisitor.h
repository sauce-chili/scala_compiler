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

    // Вывод типов выражений (возвращает nullopt если тип не удалось вывести)
    std::optional<DataType> inferExprType(ExprNode* expr);
    std::optional<DataType> inferInfixExprType(InfixExprNode* infixExpr);
    std::optional<DataType> inferPrefixExprType(PrefixExprNode* prefixExpr);
    std::optional<DataType> inferSimpleExprType(SimpleExprNode* simpleExpr);
    std::optional<DataType> inferSimpleExpr1Type(SimpleExpr1Node* simpleExpr1);
    std::optional<DataType> inferBlockStatsType(BlockStatsNode* blockStats);

    // Проверка совместимости типов при присваивании
    void checkAssignmentCompatibility(const DataType& targetType, const DataType& sourceType,
                                      int line, const std::string& context);

    // Проверка что переменная не val при присваивании
    void checkNotVal(bool isVal, const std::string& varName, int line);

    // Собрать типы аргументов из ArgumentExprsNode
    std::vector<DataType*> collectArgumentTypes(class ArgumentExprsNode* args);

    // Поиск унарного оператора как метода класса
    std::optional<DataType> resolveUnaryOperator(const std::string& op, const DataType& operandType, int line);

public:
    TypeCheckVisitor() = default;
    void visitTree(TopStatSeqNode* root);
};

#endif //SCALA_LEXER_TYPECHECKVISITOR_H
