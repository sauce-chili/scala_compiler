#include "FuncParamNode.h"

FuncParamNode::FuncParamNode() {

}

FuncParamNode *FuncParamNode::createClassParam(IdNode *id, CompoundTypeNode *compoundType, AssignExprNode *assignExpr) {
    FuncParamNode* node = new FuncParamNode();
    node->id = id;
    node->compoundType = compoundType;
    node->assignExpr = assignExpr;
    return node;
}

string FuncParamNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, id, "id_");
    addDotChild(dot, compoundType, "compoundType_");
    addDotChild(dot, assignExpr, "assignExpr_");

    return dot;
}

string FuncParamNode::getDotLabel() const {
    return "Func param";
}
