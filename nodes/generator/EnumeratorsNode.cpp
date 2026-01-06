#include "EnumeratorsNode.h"

EnumeratorsNode::EnumeratorsNode(GeneratorNode *generator) {
    enumerators = new std::list<EnumeratorPartNode*>;
    this->generator = generator;
}

EnumeratorsNode *EnumeratorsNode::addModifierToList(EnumeratorsNode *list, EnumeratorPartNode *enumeratorPart) {
    list->enumerators->push_back(enumeratorPart);
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
