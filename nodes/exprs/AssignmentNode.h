#ifndef SCALA_LEXER_ASSIGNMENTNODE_H
#define SCALA_LEXER_ASSIGNMENTNODE_H

#include "../Node.h"
#include "IExpr.h"

class ExprNode;
class IdNode;
class SimpleExprNode;
class SimpleExpr1Node;
class ArgumentExprsNode;

class AssignmentNode: public Node, public IExpr {
public:
    IdNode* fullId;
    ExprNode* expr;
    SimpleExprNode* simpleExpr;
    SimpleExpr1Node* simpleExpr1;
    ArgumentExprsNode* argumentExprs;

    AssignmentNode();

    AssignmentNode* copy();

    static AssignmentNode* createAssignment(SimpleExprNode *simpleExpr, ExprNode *expr);
    static AssignmentNode* createIdAssignment(IdNode* fullId, ExprNode* expr);
    static AssignmentNode* createFieldAssignment(SimpleExprNode* simpleExpr, IdNode* fullId, ExprNode* expr);
    static AssignmentNode* createArrayAssignment(SimpleExpr1Node* simpleExpr1, ArgumentExprsNode* argumentExprs, ExprNode* expr);

    /**
     * @brief Определяет тип данного выражения присваивания.
     * @see IExpr::inferType
     */
    DataType inferType(
        ClassMetaInfo* currentClass,
        MethodMetaInfo* currentMethod,
        Scope* currentScope
    ) const override;

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_ASSIGNMENTNODE_H
