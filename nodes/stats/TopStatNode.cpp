#include "TopStatNode.h"

TopStatNode::TopStatNode() {
    templateDef = nullptr;
    tmplDef = nullptr;
}

TopStatNode *TopStatNode::createClass(ModifiersNode *modifiers, TemplateDefNode* templateDef) {
    TopStatNode* node = new TopStatNode();
    node->templateDef = templateDef;
    node->templateDef = templateDef->setModifiers(modifiers);
    return node;
}

string TopStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, templateDef, "templateDef_");

    return dot;
}

string TopStatNode::getDotLabel() const {
    return "Top statement";
}

list<Node *> TopStatNode::getChildren() const {
    return {tmplDef};
}
