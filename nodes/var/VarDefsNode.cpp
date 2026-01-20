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
