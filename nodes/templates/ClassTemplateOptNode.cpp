#include "ClassTemplateOptNode.h"

ClassTemplateOptNode::ClassTemplateOptNode() {
    templateStats = nullptr;
    classParents  = nullptr;
}

ClassTemplateOptNode* ClassTemplateOptNode::addFuncParamToBackToList(TemplateStatsNode* templateStats) {
    ClassTemplateOptNode* node = new ClassTemplateOptNode();
    node->templateStats = templateStats;
    return node;
}

ClassTemplateOptNode *ClassTemplateOptNode::createClassTemplate(ClassParentsNode *classParents, TemplateStatsNode *templateStats) {
    ClassTemplateOptNode* node = new ClassTemplateOptNode();
    node->classParents = classParents;
    node->templateStats = templateStats;
    return node;
}

string ClassTemplateOptNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, templateStats, "templateStats_");

    return dot;
}

string ClassTemplateOptNode::getDotLabel() const {
    return "Class template optional";
}
