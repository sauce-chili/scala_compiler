#include "FuncParamNode.h"

FuncParamNode::FuncParamNode() {
    fullId = nullptr;
    compoundType = nullptr;
}

FuncParamNode *FuncParamNode::createFuncParam(IdNode *fullId, CompoundTypeNode *compoundType) {
    FuncParamNode *node = new FuncParamNode();
    node->fullId = fullId;
    node->compoundType = compoundType;
    return node;
}

string FuncParamNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, compoundType, "compoundType_");

    return dot;
}

string FuncParamNode::getDotLabel() const {
    return "Func param";
}

list<Node *> FuncParamNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, compoundType);
    return children;
}
