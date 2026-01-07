#include "TraitTemplateNode.h"
#include "../type/SimpleTypeNode.h"
#include "../type/SimpleTypesNode.h"
#include "../templates/TemplateStatsNode.h"

TraitTemplateNode::TraitTemplateNode() {

}

TraitTemplateNode *TraitTemplateNode::createTraitTemplate(SimpleTypeNode *simpleType, SimpleTypesNode *simpleTypes,
                                                          TemplateStatsNode *templateStats) {
    TraitTemplateNode* node = new TraitTemplateNode();
    node->simpleType = simpleType;
    node->simpleTypes = simpleTypes;
    node->templateStats = templateStats;
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
