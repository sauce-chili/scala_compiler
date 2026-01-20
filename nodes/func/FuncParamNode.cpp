#include "FuncParamNode.h"

FuncParamNode::FuncParamNode() {
    fullId = nullptr;
    simpleType = nullptr;
}

FuncParamNode *FuncParamNode::createFuncParam(IdNode *fullId, SimpleTypeNode* simpleType) {
    FuncParamNode* node = new FuncParamNode();
    node->fullId = fullId;
    node->simpleType = simpleType;
    return node;
}

string FuncParamNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, simpleType, "simpleType_");

    return dot;
}

string FuncParamNode::getDotLabel() const {
    return "Func param";
}
