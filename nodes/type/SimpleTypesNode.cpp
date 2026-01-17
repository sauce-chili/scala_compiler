#include "SimpleTypesNode.h"

SimpleTypesNode::SimpleTypesNode() {
    simpleTypes = new std::list<SimpleTypeNode*>;
}

SimpleTypesNode::SimpleTypesNode(SimpleTypeNode *simpleType) {
    simpleTypes = new std::list<SimpleTypeNode*>;
    if (simpleType) {
        simpleTypes->push_back(simpleType);
    }
}

SimpleTypesNode *SimpleTypesNode::addSimpleTypeToList(SimpleTypesNode *list, SimpleTypeNode *simpleType) {
    if (list == nullptr) {
        return new SimpleTypesNode(simpleType);
    }

    if (simpleType) {
        list->simpleTypes->push_back(simpleType);
    }
    return list;
}

string SimpleTypesNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!simpleTypes->empty()) {
        for (const auto *it : *simpleTypes) {
            addDotChild(dot, it, "simpleType_" + to_string(it->id));
        }
    }

    return dot;
}

string SimpleTypesNode::getDotLabel() const {
    return "Simple types";
}

list<Node *> SimpleTypesNode::getChildren() const {
    list<Node *> children = {};
    for (Node* node: *simpleTypes) {
        addChildIfNotNull(children, node);
    }
}
