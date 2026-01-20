#include "TemplateDefNode.h"
#include "../class/ClassDefNode.h"

TemplateDefNode::TemplateDefNode() {
    classDef = nullptr;
}

TemplateDefNode *TemplateDefNode::createClassDef(ClassDefNode *classDef) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = _CLASS;
    node->classDef = classDef;
    return node;
}

string TemplateDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, classDef, "classDef_");

    return dot;
}

string TemplateDefNode::getDotLabel() const {
    return "Template definition of " + statTypeToString(type);
}
