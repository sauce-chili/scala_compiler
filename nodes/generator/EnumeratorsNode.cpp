#include "EnumeratorsNode.h"

EnumeratorsNode::EnumeratorsNode() {
    enumerators = new std::list<EnumeratorPartNode*>;
}

EnumeratorsNode::EnumeratorsNode(GeneratorNode *generator) {
    enumerators = new std::list<EnumeratorPartNode*>;
    if (generator) {
        this->generator = generator;
    }
}

EnumeratorsNode *EnumeratorsNode::addModifierToList(EnumeratorsNode *list, EnumeratorPartNode *enumeratorPart) {
    if (enumeratorPart) {
        list->enumerators->push_back(enumeratorPart);
    }
    return list;
}

string EnumeratorsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!enumerators->empty()) {
        for (const auto *it : *enumerators) {
            addDotChild(dot, it, "enumerator_" + to_string(it->id));
        }
    }

    return dot;
}

string EnumeratorsNode::getDotLabel() const {
    return "Enumerators";
}

list<Node *> EnumeratorsNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, generator);
    for (Node* node : *enumerators) {
        addChildIfNotNull(children, node);
    }
    return children;
}


EnumeratorsNode *EnumeratorsNode::copy() {
    EnumeratorsNode* node = new EnumeratorsNode();

    if (enumerators) {
        node->enumerators = new std::list<EnumeratorPartNode*>();

        for (EnumeratorPartNode* e: *enumerators) {
            if (e)
                node->enumerators->push_back(e->copy());
            else
                node->enumerators->push_back(nullptr);
        }
    }

    return node;
}
