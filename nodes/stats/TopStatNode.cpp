#include "TopStatNode.h"

TopStatNode::TopStatNode() {
    tmplDef = nullptr;
}

TopStatNode *TopStatNode::createTopStat(ModifiersNode *modifiers, TemplateDefNode *tmplDef) {
    TopStatNode *node = new TopStatNode();
    node->tmplDef = tmplDef->setModifiers(modifiers);
    return node;
}


string TopStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    // addDotChild(dot, modifiers, "modifiers_");
    addDotChild(dot, tmplDef, "tmplDef_");

    return dot;
}

string TopStatNode::getDotLabel() const {
    return "Top statement";
}

list<Node *> TopStatNode::getChildren() const {
    return {tmplDef};
}
