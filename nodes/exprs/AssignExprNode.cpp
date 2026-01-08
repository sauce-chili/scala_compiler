#include "AssignExprNode.h"

AssignExprNode::AssignExprNode() {
    expr = nullptr;
}

AssignExprNode *AssignExprNode::createAssignExprNode(ExprNode *expr) {
    AssignExprNode* node = new AssignExprNode();
    node->expr = expr;
    return node;
}

string AssignExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, expr, "Expr");

    return dot;
}

string AssignExprNode::getDotLabel() const {
    return "Assign expr";
}
