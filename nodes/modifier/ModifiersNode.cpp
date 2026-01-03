#include "ModifiersNode.h"

ModifiersNode::ModifiersNode() {
    modifiers = new std::list<ModifierNode*>;
}

ModifiersNode::ModifiersNode(ModifierNode *modifier) {
    modifiers = new std::list<ModifierNode*>;
    modifiers->push_back(modifier);
}

ModifiersNode *ModifiersNode::addModifierToList(ModifiersNode *list, ModifierNode *modifier) {
    if (list == nullptr) {
        return new ModifiersNode(modifier);
    }

    list->modifiers->push_back(modifier);
    return list;
}

string ModifiersNode::toDot() const {
    return "Modifiers";
}

string ModifiersNode::getDotLabel() const {
    string dot;

    addDotNode(dot);
    if (!modifiers->empty()) {
        for (const auto *it : *modifiers) {
            addDotChild(dot, it, "modifier_" + to_string(it->id));
        }
    }

    return dot;
}
