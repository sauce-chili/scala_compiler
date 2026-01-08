#include "ModifiersNode.h"

ModifiersNode::ModifiersNode() {
    modifiers = new std::list<ModifierNode*>;
}

ModifiersNode::ModifiersNode(ModifierNode *modifier) {
    modifiers = new std::list<ModifierNode*>;
    if (modifier) {
        modifiers->push_back(modifier);
    }
}

ModifiersNode *ModifiersNode::addModifierToList(ModifiersNode *list, ModifierNode *modifier) {
    if (list == nullptr) {
        return new ModifiersNode(modifier);
    }

    list->modifiers->push_back(modifier);
    return list;
}

string ModifiersNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!modifiers->empty()) {
        for (const auto *it : *modifiers) {
            addDotChild(dot, it, "modifier_" + to_string(it->id));
        }
    }

    return dot;
}

string ModifiersNode::getDotLabel() const {
    return "Modifiers";
}
