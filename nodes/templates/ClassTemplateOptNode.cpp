#include "ClassTemplateOptNode.h"
#include "../modifier/ModifiersNode.h"

ClassTemplateOptNode::ClassTemplateOptNode() {
    templateStats = nullptr;
    extensionPartClassTemplate  = nullptr;
    modifiers = nullptr;
}

ClassTemplateOptNode* ClassTemplateOptNode::addFuncParamToBackToList(ExtensionClassTemplateNode* classTemplate, TemplateStatsNode* templateStats) {
    ClassTemplateOptNode* node = new ClassTemplateOptNode();
    node->extensionPartClassTemplate = classTemplate;
    node->templateStats = templateStats;
    return node;
}

ClassTemplateOptNode *ClassTemplateOptNode::createClassWithoutInherit(TemplateStatsNode *templateStats) {
    ClassTemplateOptNode* node = new ClassTemplateOptNode();
    node->templateStats = templateStats;
    return node;
}

ClassTemplateOptNode *ClassTemplateOptNode::copy() {
    ClassTemplateOptNode* node = new ClassTemplateOptNode();

    if (extensionPartClassTemplate) {
        node->extensionPartClassTemplate = extensionPartClassTemplate->copy();
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
    addDotChild(dot, extensionPartClassTemplate, "extension part of class_");

    return dot;
}

string ClassTemplateOptNode::getDotLabel() const {
    return "Class template optional";
}

list<Node *> ClassTemplateOptNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, extensionPartClassTemplate);
    addChildIfNotNull(children, templateStats);
    return children;
}

ClassTemplateOptNode *ClassTemplateOptNode::setModifiers(ModifiersNode *modifiers) {
    this->modifiers = modifiers;
    return this;
}

