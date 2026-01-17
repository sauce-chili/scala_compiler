#include "EnumStatsNode.h"

EnumStatsNode::EnumStatsNode() {
    enumStats = new std::list<EnumStatNode*>;
}

EnumStatsNode::EnumStatsNode(EnumStatNode *enumStat) {
    enumStats = new std::list<EnumStatNode*>;
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

EnumStatsNode *EnumStatsNode::copy() {
    EnumStatsNode* copied = new EnumStatsNode();

    if (enumStats) {
        copied->enumStats = new std::list<EnumStatNode*>();

        for (EnumStatNode* es: *enumStats) {
            if (es)
                copied->enumStats->push_back(es->copy());
            else
                copied->enumStats->push_back(nullptr);
        }
    }

    return copied;
}

string EnumStatsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!enumStats->empty()) {
        for (const auto *it : *enumStats) {
            addDotChild(dot, it, "enumStat_" + to_string(it->id));
        }
    }

    return dot;
}

string EnumStatsNode::getDotLabel() const {
    return "Enum stats";
}
