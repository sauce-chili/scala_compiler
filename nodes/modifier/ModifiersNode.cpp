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

    if (modifier) {
        list->modifiers->push_back(modifier);
    }
    return list;
}

ModifiersNode *ModifiersNode::copy() {
    ModifiersNode* copied = new ModifiersNode();

    if (modifiers) {
        copied->modifiers = new std::list<ModifierNode*>();

        for (ModifierNode* m: *modifiers) {
            if (m)
                copied->modifiers->push_back(m->copy());
            else
                copied->modifiers->push_back(nullptr);
        }
    }

    return copied;
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
