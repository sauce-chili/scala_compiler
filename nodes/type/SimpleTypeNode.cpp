#include "SimpleTypeNode.h"

SimpleTypeNode::SimpleTypeNode() {
    fullId = nullptr;
    simpleType = nullptr;
}

SimpleTypeNode *SimpleTypeNode::createIdTypeNode(IdNode *fullId) {
    SimpleTypeNode* node = new SimpleTypeNode();
    node->fullId = fullId;
    return node;
}

SimpleTypeNode *SimpleTypeNode::createSimpleTypeNode(SimpleTypeNode *simpleType) {
    SimpleTypeNode* node = new SimpleTypeNode();
    node->simpleType = simpleType;
    return node;
}

string SimpleTypeNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "id");
    addDotChild(dot, simpleType, "Array simple type");

    return dot;
}

string SimpleTypeNode::getDotLabel() const {
    return this->fullId ? "Simple type is id" : "Simple type is array";
}
