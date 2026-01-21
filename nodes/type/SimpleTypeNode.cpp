#include "SimpleTypeNode.h"

SimpleTypeNode::SimpleTypeNode() {
    fullId = nullptr;
    arrayElemType = nullptr;
}

SimpleTypeNode *SimpleTypeNode::createIdTypeNode(IdNode *fullId) {
    SimpleTypeNode* node = new SimpleTypeNode();
    node->fullId = fullId;
    return node;
}

SimpleTypeNode *SimpleTypeNode::createSimpleTypeNode(SimpleTypeNode *simpleType) {
    SimpleTypeNode* node = new SimpleTypeNode();
    node->arrayElemType = simpleType;
    return node;
}

SimpleTypeNode *SimpleTypeNode::copy() {
    SimpleTypeNode* node = new SimpleTypeNode();

    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (arrayElemType) {
        node->arrayElemType = arrayElemType->copy();
    }

    return node;
}

string SimpleTypeNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "id");
    addDotChild(dot, arrayElemType, "Array simple type");

    return dot;
}

string SimpleTypeNode::getDotLabel() const {
    return this->fullId ? "Simple type is id" : "Simple type is array";
}

list<Node *> SimpleTypeNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, arrayElemType);
    return children;
}
