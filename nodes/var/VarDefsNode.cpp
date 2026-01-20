#include "VarDefsNode.h"

VarDefsNode::VarDefsNode() {
    ids = nullptr;
    simpleType = nullptr;
    expr = nullptr;
}

VarDefsNode *VarDefsNode::createVal(IdsNode* ids, SimpleTypeNode* simpleType, ExprNode* expr) {
    VarDefsNode* node = new VarDefsNode();
    node->type = _VAL_DECL;
    node->ids = ids;
    node->simpleType = simpleType;
    node->expr = expr;
    return node;
}

VarDefsNode *VarDefsNode::createVar(IdsNode* ids, SimpleTypeNode* simpleType, ExprNode* expr) {
    VarDefsNode* node = new VarDefsNode();
    node->type = _VAR_DECL;
    node->ids = ids;
    node->simpleType = simpleType;
    node->expr = expr;
    return node;
}

VarDefsNode *VarDefsNode::copy() {
    VarDefsNode* node = new VarDefsNode();
    node->type = type;

    if (ids) {
        node->ids = ids->copy();
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
    addDotChild(dot, ids, "Ids");
    addDotChild(dot, simpleType, "Simple type");
    addDotChild(dot, expr, "Expr");

    return dot;
}

string VarDefsNode::getDotLabel() const {
    return statTypeToString(type);
}

list<Node *> VarDefsNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, ids);
    addChildIfNotNull(children, simpleType);
    addChildIfNotNull(children, expr);
    return children;
}

