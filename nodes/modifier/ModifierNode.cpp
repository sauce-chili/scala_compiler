#include "ModifierNode.h"

ModifierNode* ModifierNode::createModifier(ModifierType type, ModifierType m) {
    ModifierNode* node = new ModifierNode();
    node->type = type;
    node->modifier = m;
    return node;
}

string ModifierNode::getDotLabel() const {
    string dot;

    dot += "modifier: " + modifierToString(modifier);

    return dot;
}

string ModifierNode::toDot() const {
    string dot;

    addDotNode(dot);

    return dot;
}