#include "TopStatNode.h"

TopStatNode::TopStatNode() {
    modifiers = nullptr;
    templateDef = nullptr;
}

TopStatNode *TopStatNode::createClass(ModifiersNode *modifiers, TemplateDefNode* templateDef) {
    TopStatNode* node = new TopStatNode();
    node->modifiers = modifiers;
    node->templateDef = templateDef;
    return node;
}

string TopStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, modifiers, "modifiers_");
    addDotChild(dot, templateDef, "templateDef_");

    return dot;
}

string TopStatNode::getDotLabel() const {
    return "Top statement";
}
