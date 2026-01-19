#include "TraitTemplateOptNode.h"
#include "../templates/TemplateStatsNode.h"

TraitTemplateOptNode::TraitTemplateOptNode() {
    traitTemplate = nullptr;
    templateStats = nullptr;
}

TraitTemplateOptNode *TraitTemplateOptNode::createTraitTemplateOpt(TraitTemplateNode *traitTemplate, TemplateStatsNode *templateStats) {
    TraitTemplateOptNode* node = new TraitTemplateOptNode();
    node->traitTemplate = traitTemplate;
    node->templateStats = templateStats;
    return node;
}

TraitTemplateOptNode *TraitTemplateOptNode::copy() {
    TraitTemplateOptNode* node = new TraitTemplateOptNode();

    if (traitTemplate) {
        node->traitTemplate = traitTemplate->copy();
    }
    if (templateStats) {
        node->templateStats = templateStats->copy();
    }

    return node;
}

string TraitTemplateOptNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, traitTemplate, "traitTemplate_");
    addDotChild(dot, templateStats, "templateStats_");

    return dot;
}

string TraitTemplateOptNode::getDotLabel() const {
    return "Trait template opt";
}

