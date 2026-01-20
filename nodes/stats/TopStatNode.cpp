#include "TopStatNode.h"

TopStatNode::TopStatNode() {
    modifiers = nullptr;
    classDef = nullptr;
}

TopStatNode *TopStatNode::createClass(ModifiersNode *modifiers, ClassDefNode* classDef) {
    TopStatNode* node = new TopStatNode();
    node->modifiers = modifiers;
    node->classDef = classDef;
    return node;
}

string TopStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, modifiers, "modifiers_");
    addDotChild(dot, classDef, "classDef_");

    return dot;
}

string TopStatNode::getDotLabel() const {
    return "Top statement";
}
