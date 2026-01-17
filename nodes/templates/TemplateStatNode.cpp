#include "TemplateStatNode.h"
#include "../definitions/DclNode.h"
#include "../definitions/DefNode.h"

TemplateStatNode::TemplateStatNode() {
    def = nullptr;
    dcl = nullptr;
}

TemplateStatNode *TemplateStatNode::createDefTemplate(ModifiersNode *modifiers, DefNode *def) {
    TemplateStatNode* node = new TemplateStatNode();
    node->def = def;
    node->def->setModifiers(modifiers);
    return node;
}

TemplateStatNode *TemplateStatNode::createDclTemplate(ModifiersNode *modifiers, DclNode *dcl) {
    TemplateStatNode* node = new TemplateStatNode();
    node->dcl = dcl;
    node->dcl->setModifiers(modifiers);
    return node;
}

string TemplateStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (def) {
        addDotChild(dot, def, "def_");
        addDotChild(dot, def->modifiers, "modifiers_");
    } else {
        addDotChild(dot, dcl, "dcl_");
        addDotChild(dot, dcl->modifiers, "modifiers_");
    }

    return dot;
}

string TemplateStatNode::getDotLabel() const {
    return "Template stat";
}

list<Node *> TemplateStatNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, dcl);
    addChildIfNotNull(children, def);
    return children;
}

