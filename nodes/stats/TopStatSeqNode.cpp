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

TopStatSeqNode *TopStatSeqNode::copy() {
    TopStatSeqNode* copied = new TopStatSeqNode();

    if (topStats) {
        copied->topStats = new std::list<TopStatNode*>();

        for (TopStatNode* e: *topStats) {
            if (e)
                copied->topStats->push_back(e->copy());
            else
                copied->topStats->push_back(nullptr);
        }
    }

    return copied;
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

list<Node *> TopStatSeqNode::getChildren() const {
    list<Node *> children = {};
    for (Node *node : *topStats) {
        addChildIfNotNull(children, node);
    }
    return children;
}

