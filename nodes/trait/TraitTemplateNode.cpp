#include "TraitTemplateNode.h"
#include "../type/SimpleTypeNode.h"
#include "../type/SimpleTypesNode.h"
#include "../templates/TemplateStatsNode.h"

TraitTemplateNode::TraitTemplateNode() {
    simpleType = nullptr;
    simpleTypes = nullptr;
    templateStats = nullptr;
}

TraitTemplateNode *TraitTemplateNode::createTraitTemplate(SimpleTypeNode *simpleType, SimpleTypesNode *simpleTypes,
                                                          TemplateStatsNode *templateStats) {
    TraitTemplateNode* node = new TraitTemplateNode();
    node->simpleType = simpleType;
    node->simpleTypes = simpleTypes;
    node->templateStats = templateStats;
    return node;
}

TraitTemplateNode* TraitTemplateNode::copy() {
    TraitTemplateNode* node = new TraitTemplateNode();

    if (simpleType) {
        node->simpleType = simpleType->copy();
    }
    if (simpleTypes) {
        node->simpleTypes = simpleTypes->copy();
    }
    if (templateStats) {
        node->templateStats = templateStats->copy();
    }

    return node;
}

string TraitTemplateNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, simpleType, "simpleType_");
    addDotChild(dot, simpleTypes, "simpleTypes_");
    addDotChild(dot, templateStats, "templateStats_");

    return dot;
}

string TraitTemplateNode::getDotLabel() const {
    return "Trait template";
}
