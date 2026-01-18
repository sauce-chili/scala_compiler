#include "SimpleTypeNode.h"

SimpleTypeNode::SimpleTypeNode() {
    stableId = nullptr;
    compoundType = nullptr;
}

SimpleTypeNode *SimpleTypeNode::createStableIdNode(StableIdNode *stableId) {
    SimpleTypeNode* node = new SimpleTypeNode();
    node->stableId = stableId;
    return node;
}

SimpleTypeNode *SimpleTypeNode::createArrayWithCompoundTypeNode(CompoundTypeNode *compoundType) {
    SimpleTypeNode* node = new SimpleTypeNode();
    node->compoundType = compoundType;
    return node;
}

SimpleTypeNode *SimpleTypeNode::copy() {
    SimpleTypeNode* node = new SimpleTypeNode();

    if (stableId) {
        node->stableId = stableId->copy();
    }
    if (compoundType) {
        node->compoundType = compoundType->copy();
    }

    return node;
}

string SimpleTypeNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, stableId, "Stable id");
    addDotChild(dot, compoundType, "Array compound");

    return dot;
}

string SimpleTypeNode::getDotLabel() const {
    return this->stableId ? "Simple type is stable id" : "Simple type is array";
}

list<Node *> SimpleTypeNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, stableId);
    addChildIfNotNull(children, compoundType);
    return children;
}
