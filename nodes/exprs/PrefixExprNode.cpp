#include "PrefixExprNode.h"

PrefixExprNode::PrefixExprNode() {
    simpleExpr = nullptr;
}

PrefixExprNode *PrefixExprNode::createPrefixExprNode(SimpleExprNode *simpleExpr, PrefixExprType type) {
    PrefixExprNode* node = new PrefixExprNode();
    node->type = type;
    node->simpleExpr = simpleExpr;
    return node;
}

PrefixExprNode *PrefixExprNode::copy() {
    PrefixExprNode* node = new PrefixExprNode();

    node->type = type;

    if (simpleExpr) {
        node->simpleExpr = simpleExpr->copy();
    }

    return node;
}

string PrefixExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, simpleExpr, "Simple expr");

    return dot;
}

string PrefixExprNode::getDotLabel() const {
    return prefixExprToString(type) + " prefix expr";
}

list<Node *> PrefixExprNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, simpleExpr);
    return children;
}
