#include "TopStatSeqNode.h"

TopStatSeqNode::TopStatSeqNode() {
    topStats = new std::list<TopStatNode*>;
}

TopStatSeqNode::TopStatSeqNode(TopStatNode *topStat) {
    topStats = new std::list<TopStatNode*>;
    if (topStat) {
        topStats->push_back(topStat);
    }
}

TopStatSeqNode *TopStatSeqNode::addModifierToList(TopStatSeqNode *list, TopStatNode *topStat) {
    if (list == nullptr) {
        return new TopStatSeqNode(topStat);
    }

    if (topStat) {
        list->topStats->push_back(topStat);
    }
    return list;
}

string TopStatSeqNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!topStats->empty()) {
        for (const auto *it : *topStats) {
            addDotChild(dot, it, "topStat_" + to_string(it->id));
        }
    }

    return dot;
}

string TopStatSeqNode::getDotLabel() const {
    return "Top statement sequence";
}
