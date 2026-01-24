#include "VarDefsNode.h"

VarDefsNode::VarDefsNode() {
    fullId = nullptr;
    simpleType = nullptr;
    expr = nullptr;
}

VarDefsNode *VarDefsNode::createVal(IdNode* fullId, SimpleTypeNode* simpleType, ExprNode* expr) {
    VarDefsNode* node = new VarDefsNode();
    node->type = _VAL_DECL;
    node->fullId = fullId;
    node->simpleType = simpleType;
    node->expr = expr;
    return node;
}

VarDefsNode *VarDefsNode::createVar(IdNode* fullId, SimpleTypeNode* simpleType, ExprNode* expr) {
    VarDefsNode* node = new VarDefsNode();
    node->type = _VAR_DECL;
    node->fullId = fullId;
    node->simpleType = simpleType;
    node->expr = expr;
    return node;
}

VarDefsNode *VarDefsNode::copy() {
    VarDefsNode* node = new VarDefsNode();
    node->type = type;

    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (simpleType) {
        node->simpleType = simpleType->copy();
    }
    if (expr) {
        node->expr = expr->copy();
    }

    return node;
}

string VarDefsNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "Id");
    addDotChild(dot, simpleType, "Simple type");
    addDotChild(dot, expr, "Expr");

    return dot;
}

string VarDefsNode::getDotLabel() const {
    return statTypeToString(type);
}

list<Node *> VarDefsNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, simpleType);
    addChildIfNotNull(children, expr);
    return children;
}

