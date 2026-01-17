#include "TopStatNode.h"

TopStatNode::TopStatNode() {
    modifiers = nullptr;
    tmplDef = nullptr;
}

TopStatNode *TopStatNode::createTopStat(ModifiersNode *modifiers, TemplateDefNode *tmplDef) {
    TopStatNode* node = new TopStatNode();
    node->modifiers = modifiers;
    node->tmplDef = tmplDef;
    return node;
}

TopStatNode *TopStatNode::copy() {
    TopStatNode* node = new TopStatNode();

    if (modifiers) {
        node->modifiers = modifiers->copy();
    }
    if (tmplDef) {
        node->tmplDef = tmplDef->copy();
    }

    return node;
}

string TopStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, modifiers, "modifiers_");
    addDotChild(dot, tmplDef, "tmplDef_");

    return dot;
}

string TopStatNode::getDotLabel() const {
    return "Top statement";
}
