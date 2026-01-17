#include "VarDefsNode.h"

VarDefsNode::VarDefsNode() {
    ids = nullptr;
    compoundType = nullptr;
    expr = nullptr;
}

VarDefsNode *VarDefsNode::createVal(IdsNode* ids, CompoundTypeNode* compoundType, ExprNode* expr) {
    VarDefsNode* node = new VarDefsNode();
    node->type = _VAL_DECL;
    node->ids = ids;
    node->compoundType = compoundType;
    node->expr = expr;
    return node;
}

VarDefsNode *VarDefsNode::createVar(IdsNode* ids, CompoundTypeNode* compoundType, ExprNode* expr) {
    VarDefsNode* node = new VarDefsNode();
    node->type = _VAR_DECL;
    node->ids = ids;
    node->compoundType = compoundType;
    node->expr = expr;
    return node;
}

VarDefsNode *VarDefsNode::copy() {
    VarDefsNode* node = new VarDefsNode();
    node->type = type;

    if (ids) {
        node->ids = ids->copy();
    }
    if (compoundType) {
        node->compoundType = compoundType->copy();
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
    addDotChild(dot, compoundType, "Array compound");
    addDotChild(dot, expr, "Expr");

    return dot;
}

string VarDefsNode::getDotLabel() const {
    return statTypeToString(type);
}

