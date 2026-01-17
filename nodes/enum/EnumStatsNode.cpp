#include "EnumStatsNode.h"

EnumStatsNode::EnumStatsNode() {
    enumStats = new std::list<EnumStatNode *>;
}

EnumStatsNode::EnumStatsNode(EnumStatNode *enumStat) {
    enumStats = new std::list<EnumStatNode *>;
    if (enumStat) {
        enumStats->push_back(enumStat);
    }
}

EnumStatsNode *EnumStatsNode::addModifierToList(EnumStatsNode *list, EnumStatNode *enumStat) {
    if (list == nullptr) {
        return new EnumStatsNode(enumStat);
    }

    if (enumStat) {
        list->enumStats->push_back(enumStat);
    }
    return list;
}

string EnumStatsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!enumStats->empty()) {
        for (const auto *it: *enumStats) {
            addDotChild(dot, it, "enumStat_" + to_string(it->id));
        }
    }

    return dot;
}

string EnumStatsNode::getDotLabel() const {
    return "Enum stats";
}

list<Node *> EnumStatsNode::getChildren() const {
    list<Node *> children = {};
    for (Node *it: *enumStats) {
        addChildIfNotNull(children, it);
    }
    return children;
}
