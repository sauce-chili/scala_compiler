#include "SimpleTypeNode.h"

SimpleTypeNode::SimpleTypeNode() {

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
