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

ClassTemplateOptNode *ClassTemplateOptNode::copy() {
    ClassTemplateOptNode* node = new ClassTemplateOptNode();

    if (classParents) {
        node->classParents = classParents->copy();
    }
    if (templateStats) {
        node->templateStats = templateStats->copy();
    }

    return node;
}

string ClassTemplateOptNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, templateStats, "templateStats_");
    addDotChild(dot, classParents, "classParents_");

    return dot;
}

string ClassTemplateOptNode::getDotLabel() const {
    return "Class template optional";
}

list<Node *> ClassTemplateOptNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, classParents);
    addChildIfNotNull(children, templateStats);
    return children;
}

