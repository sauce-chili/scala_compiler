#ifndef SCALA_LEXER_ASSIGNMENTNODE_H
#define SCALA_LEXER_ASSIGNMENTNODE_H

#include "ExprNode.h"
#include "../id/IdNode.h"
#include "SimpleExprNode.h"
#include "SimpleExpr1Node.h"

class AssignmentNode: public Node {
public:
    IdNode* fullId;
    ExprNode* expr;
    SimpleExprNode* simpleExpr;
    SimpleExpr1Node* simpleExpr1;
    ArgumentExprsNode* argumentExprs;

    AssignmentNode();

    static AssignmentNode* createIdAssignment(IdNode* fullId, ExprNode* expr);
    static AssignmentNode* createFieldAssignment(SimpleExprNode* simpleExpr, IdNode* fullId, ExprNode* expr);
    static AssignmentNode* createArrayAssignment(SimpleExpr1Node* simpleExpr1, ArgumentExprsNode* argumentExprs, ExprNode* expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ASSIGNMENTNODE_H
