#include "ModifierNode.h"

ModifierNode* ModifierNode::createModifier(ModifierType type) {
    ModifierNode* node = new ModifierNode();
    node->type = type;
    return node;
}

ModifierNode *ModifierNode::copy() {
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

bool ModifierNode::isAccessModifier() const {
    return type == _PUBLIC
        || type == _PRIVATE
        || type == _PROTECTED;
}

bool ModifierNode::isOverrideModifier() const {
    return type == _OVERRIDE;

}

bool ModifierNode::isInheritModifier() const {
    return type == _ABSTRACT
        || type == _FINAL
        || type == _SEALED;
}

