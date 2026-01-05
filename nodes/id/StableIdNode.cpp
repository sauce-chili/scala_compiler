#include "StableIdNode.h"

StableIdNode::StableIdNode(SingleStableIdNode *first) {
    stableIds = new std::list<SingleStableIdNode*>();
    stableIds->push_back(first);
}

StableIdNode *StableIdNode::addStableId(StableIdNode *list, SingleStableIdNode *stableId) {
    if (list == nullptr) {
        return new StableIdNode(stableId);
    }

    list->stableIds->push_back(stableId);
    return list;
}

string StableIdNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!stableIds->empty()) {
        int i = 0;
        for (const auto *it : *stableIds) {
            addDotChild(dot, it, "stableId_" + to_string(i));
            i++;
        }
    }

    return dot;
}

string StableIdNode::getDotLabel() const {
    return "Stable id";
}
