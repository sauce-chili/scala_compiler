#include "TemplateStatNode.h"
#include "../definitions/DclNode.h"
#include "../definitions/DefNode.h"
#include "../modifier/ModifiersNode.h"

TemplateStatNode::TemplateStatNode() {

}

TemplateStatNode *TemplateStatNode::createDefTemplate(ModifiersNode *modifiers, DefNode *def) {
    TemplateStatNode* node = new TemplateStatNode();
    node->modifiers = modifiers;
    node->def = def;
    return node;
}

TemplateStatNode *TemplateStatNode::createDclTemplate(ModifiersNode *modifiers, DclNode *dcl) {
    TemplateStatNode* node = new TemplateStatNode();
    node->modifiers = modifiers;
    node->dcl = dcl;
    return node;
}

string TemplateStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, modifiers, "modifiers_");
    addDotChild(dot, def, "def_");
    addDotChild(dot, dcl, "dcl_");

    return dot;
}

string TemplateStatNode::getDotLabel() const {
    return "Template stat";
}
