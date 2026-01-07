#include "AssignmentNode.h"
#include "ExprNode.h"
#include "SimpleExprNode.h"
#include "SimpleExpr1Node.h"
#include "../exprs/ArgumentExprsNode.h"
#include "nodes/id/IdNode.h"

AssignmentNode::AssignmentNode() {

}

AssignmentNode *AssignmentNode::createIdAssignment(IdNode *fullId, ExprNode *expr) {
    AssignmentNode* node = new AssignmentNode();
    node->fullId = fullId;
    node->expr = expr;
    return node;
}

AssignmentNode *AssignmentNode::createFieldAssignment(SimpleExprNode *simpleExpr, IdNode *fullId, ExprNode *expr) {
    AssignmentNode* node = new AssignmentNode();
    node->simpleExpr = simpleExpr;
    node->fullId = fullId;
    node->expr = expr;
    return node;
}

AssignmentNode *AssignmentNode::createArrayAssignment(SimpleExpr1Node *simpleExpr1, ArgumentExprsNode *argumentExprs, ExprNode *expr) {
    AssignmentNode* node = new AssignmentNode();
    node->simpleExpr1 = simpleExpr1;
    node->argumentExprs = argumentExprs;
    node->expr = expr;
    return node;
}

string AssignmentNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, simpleExpr, "simpleExpr_");
    addDotChild(dot, expr, "expr_");
    addDotChild(dot, simpleExpr1, "simpleExpr1_");
    addDotChild(dot, argumentExprs, "argumentExprs_");

    return dot;
}

string AssignmentNode::getDotLabel() const {
    if (simpleExpr) {
        return "Field assignment";
    } else if (simpleExpr1) {
        return "Array assignment";
    }

    return "Id assignment";
}
