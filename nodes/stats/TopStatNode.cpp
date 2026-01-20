#include "TopStatNode.h"

TopStatNode::TopStatNode() {
    tmplDef = nullptr;
}

TopStatNode *TopStatNode::createClass(ModifiersNode *modifiers, TemplateDefNode* templateDef) {
    TopStatNode* node = new TopStatNode();
    node->tmplDef = templateDef;
    node->tmplDef = templateDef->setModifiers(modifiers);
    return node;
}

TopStatNode *TopStatNode::copy() {
    TopStatNode* node = new TopStatNode();

    if (tmplDef) {
        node->tmplDef = tmplDef->copy();
    }

    return node;
}

string TopStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, tmplDef, "templateDef_");

    return dot;
}

string TopStatNode::getDotLabel() const {
    return "Top statement";
}

list<Node *> TopStatNode::getChildren() const {
    return {tmplDef};
}
