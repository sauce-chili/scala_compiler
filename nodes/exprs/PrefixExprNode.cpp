#include "PrefixExprNode.h"

PrefixExprNode::PrefixExprNode() {

}

PrefixExprNode *PrefixExprNode::createPrefixExprNode(SimpleExprNode *simpleExpr, PrefixExprType type) {
    PrefixExprNode* node = new PrefixExprNode();
    node->type = type;
    node->simpleExpr = simpleExpr;
    return node;
}

string PrefixExprNode::toDot() const {
    string dot;

    addDotNode(dot);

    addDotChild(dot, simpleExpr, "Simple expr");
}

string PrefixExprNode::getDotLabel() const {
    return prefixExprToString(type) + " prefix expr";
}
