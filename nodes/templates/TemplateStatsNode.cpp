#include "TemplateStatsNode.h"
#include "../definitions/DclNode.h"

TemplateStatsNode::TemplateStatsNode() {
    templateStats = new std::list<TemplateStatNode*>;
}

TemplateStatsNode::TemplateStatsNode(TemplateStatNode *first) {
    templateStats = new std::list<TemplateStatNode*>;
    if (first) {
        templateStats->push_back(first);
    }
}

TemplateStatsNode* TemplateStatsNode::addFuncParamToBackToList(TemplateStatsNode *list, TemplateStatNode *templateStat) {
    if (list == nullptr) {
        return new TemplateStatsNode(templateStat);
    }

    if (templateStat) {
        list->templateStats->push_back(templateStat);
    }
    return list;
}

TemplateStatsNode* TemplateStatsNode::addFuncParamToFrontToList(TemplateStatsNode *list, TemplateStatNode *templateStat) {
    if (list == nullptr) {
        return new TemplateStatsNode(templateStat);
    }

    if (templateStat != nullptr) {
        list->templateStats->push_front(templateStat);
    }
    return list;
}

TemplateStatsNode *TemplateStatsNode::copy() {
    TemplateStatsNode* copied = new TemplateStatsNode();

    if (templateStats) {
        copied->templateStats = new std::list<TemplateStatNode*>();

        for (TemplateStatNode* e: *templateStats) {
            if (e)
                copied->templateStats->push_back(e->copy());
            else
                copied->templateStats->push_back(nullptr);
        }
    }

    return copied;
}

bool TemplateStatsNode::containsVar(string name) {
    for (TemplateStatNode* ts: *templateStats) {
        if (ts) {
            if (ts->dcl && ts->dcl->containsVar(name)) return true;
        }
    }

    return false;
}

string TemplateStatsNode::toDot() const {
    string dot;

    this;
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

