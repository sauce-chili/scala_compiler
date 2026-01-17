#include "CompoundTypeNode.h"
#include "nodes/id/StableIdNode.h"

CompoundTypeNode::CompoundTypeNode(SimpleTypeNode *first) {
    simpleTypes = new std::list<SimpleTypeNode*>();
    if (first) {
        simpleTypes->push_back(first);
    }
}

CompoundTypeNode::CompoundTypeNode(IdNode *first) {
    simpleTypes = new std::list<SimpleTypeNode*>();
    CompoundTypeNode(SimpleTypeNode::createStableIdNode(
                    StableIdNode::createStableIdByFullId(
                            first
                           )));
}

CompoundTypeNode::CompoundTypeNode() {
    simpleTypes = new std::list<SimpleTypeNode*>();
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

CompoundTypeNode* CompoundTypeNode::copy() {
    auto* compoundType = new CompoundTypeNode();

    if (simpleTypes) {
        compoundType->simpleTypes = new std::list<SimpleTypeNode*>();

        for (SimpleTypeNode* st : *simpleTypes) {
            if (st)
                compoundType->simpleTypes->push_back(st->copy());
            else
                compoundType->simpleTypes->push_back(nullptr);
        }
    }

    return compoundType;
}


string CompoundTypeNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!simpleTypes->empty()) {
        int i = 0;
        for (const auto *it : *simpleTypes) {
            addDotChild(dot, it, "simpleType_" + to_string(i));
            i++;
        }
    }

    return dot;
}

string CompoundTypeNode::getDotLabel() const {
    return "Compound type";
}