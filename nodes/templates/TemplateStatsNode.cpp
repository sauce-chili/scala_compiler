#include "TemplateStatsNode.h"

TemplateStatsNode::TemplateStatsNode() {
    templateStats = new std::list<TemplateStatNode*>;
}

TemplateStatsNode::TemplateStatsNode(TemplateStatNode *first) {
    templateStats = new std::list<TemplateStatNode*>;
    templateStats->push_back(first);
}

TemplateStatsNode* TemplateStatsNode::addFuncParamToBackToList(TemplateStatsNode *list, TemplateStatNode *templateStat) {
    if (list == nullptr) {
        return new TemplateStatsNode(templateStat);
    }

    list->templateStats->push_back(templateStat);
    return list;
}

TemplateStatsNode* TemplateStatsNode::addFuncParamToFrontToList(TemplateStatsNode *list, TemplateStatNode *templateStat) {
    if (list == nullptr) {
        return new TemplateStatsNode(templateStat);
    }

    list->templateStats->push_front(templateStat);
    return list;
}

string TemplateStatsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!templateStats->empty()) {
        for (const auto *it : *templateStats) {
            addDotChild(dot, it, "templateStat_" + to_string(it->id));
        }
    }

    return dot;
}

string TemplateStatsNode::getDotLabel() const {
    return "Template stats";
}
