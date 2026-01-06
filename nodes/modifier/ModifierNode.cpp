#include "ModifierNode.h"

ModifierNode* ModifierNode::createModifier(ModifierType type) {
    ModifierNode* node = new ModifierNode();
    node->type = type;
    return node;
}

string ModifierNode::getDotLabel() const {
    string dot;

    dot += "modifier: " + modifierToString(type);

    return dot;
}

string ModifierNode::toDot() const {
    string dot;

    addDotNode(dot);

    return dot;
}