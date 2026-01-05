#include "ClassTemplateNode.h"

ClassTemplateNode::ClassTemplateNode() {

}

ClassTemplateNode* ClassTemplateNode::createClassTemplate(ClassParentsNode* classParents, TemplateStatsNode* templateStats) {
    ClassTemplateNode* node = new ClassTemplateNode();
    node->classParents = classParents;
    node->templateStats = templateStats;
    return node;
}

string ClassTemplateNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, classParents, "classParents_");
    addDotChild(dot, templateStats, "templateStats_");

    return dot;
}

string ClassTemplateNode::getDotLabel() const {
    return "Class template";
}
