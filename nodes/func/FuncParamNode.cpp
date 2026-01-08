#include "FuncParamNode.h"

FuncParamNode::FuncParamNode() {
    fullId = nullptr;
    compoundType = nullptr;
    assignExpr = nullptr;
}

FuncParamNode *FuncParamNode::createClassParam(IdNode *fullId, CompoundTypeNode *compoundType, AssignExprNode *assignExpr) {
    FuncParamNode* node = new FuncParamNode();
    node->fullId = fullId;
    node->compoundType = compoundType;
    node->assignExpr = assignExpr;
    return node;
}

string FuncParamNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, compoundType, "compoundType_");
    addDotChild(dot, assignExpr, "assignExpr_");

    return dot;
}

string FuncParamNode::getDotLabel() const {
    return "Func param";
}
