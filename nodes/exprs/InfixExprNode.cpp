#include "InfixExprNode.h"

InfixExprNode::InfixExprNode() {
    left = nullptr;
    right = nullptr;
    fullId = nullptr;
    prefixExpr = nullptr;
}

InfixExprNode *InfixExprNode::createInfixFromPrefix(PrefixExprNode *prefixExpr) {
    InfixExprNode* node = new InfixExprNode();
    node->prefixExpr = prefixExpr;
    return node;
}

InfixExprNode *InfixExprNode::createFromInfixes(InfixExprNode *left, IdNode *fullId, InfixExprNode *right) {
    InfixExprNode* node = new InfixExprNode();
    node->left = left;
    node->fullId = fullId;
    node->right = right;
    return node;
}

string InfixExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, prefixExpr, "prefixExpr_");
    addDotChild(dot, left, "left_");
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, right, "right_");

    return dot;
}

string InfixExprNode::getDotLabel() const {
    return prefixExpr ? "Infix expr from prefix" : "Infix expr with two infixes";
}

list<Node *> InfixExprNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, prefixExpr);
    addChildIfNotNull(children, left);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, right);
    return children;
}
