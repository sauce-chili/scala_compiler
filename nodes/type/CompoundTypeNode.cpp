#include "CompoundTypeNode.h"

CompoundTypeNode::CompoundTypeNode(SimpleTypeNode *first) {
    simpleTypes = new std::list<SimpleTypeNode*>();
    if (first) {
        simpleTypes->push_back(first);
    }
}

CompoundTypeNode *CompoundTypeNode::addStableId(CompoundTypeNode *list, SimpleTypeNode *simpleType) {
    if (list == nullptr) {
        return new CompoundTypeNode(simpleType);
    }

    if (simpleType) {
        list->simpleTypes->push_back(simpleType);
    }
    return list;
}

string CompoundTypeNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!simpleTypes->empty()) {
        int i = 0;
        for (const auto *it : *simpleTypes) {
            addDotChild(dot, it, "compoundType_" + to_string(i));
            i++;
        }
    }

    return dot;
}

string CompoundTypeNode::getDotLabel() const {
    return "Compound type";
}


list<Node *> CompoundTypeNode::getChildren() const {
    list<Node *> children = {};
    for (Node *node : *simpleTypes) {
        addChildIfNotNull(children, node);
    }
    return children;
}
