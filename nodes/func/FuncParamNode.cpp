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

FuncParamNode *FuncParamNode::copy() {
    FuncParamNode* node = new FuncParamNode();

    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (simpleType) {
        node->simpleType = simpleType->copy();
    }

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

list<Node *> FuncParamNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, simpleType);
    return children;
}
