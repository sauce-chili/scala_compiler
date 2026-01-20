#include "ClassTemplateOptNode.h"

ClassTemplateOptNode::ClassTemplateOptNode() {
    classTemplate = nullptr;
    templateStats = nullptr;
}

ClassTemplateOptNode* ClassTemplateOptNode::addFuncParamToBackToList(ExtensionClassTemplateNode* classTemplate, TemplateStatsNode* templateStats) {
    ClassTemplateOptNode* node = new ClassTemplateOptNode();
    node->classTemplate = classTemplate;
    node->templateStats = templateStats;
    return node;
}

string ClassTemplateOptNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, classTemplate, "classTemplate_");
    addDotChild(dot, templateStats, "templateStats_");

    return dot;
}

string ClassTemplateOptNode::getDotLabel() const {
    return "Class template optional";
}
