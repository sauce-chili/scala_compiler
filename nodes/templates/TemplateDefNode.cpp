#include "TemplateDefNode.h"
#include "../class/ClassDefNode.h"

TemplateDefNode::TemplateDefNode() {
    classDef = nullptr;
}

TemplateDefNode *TemplateDefNode::createClassDef(ClassDefNode *classDef) {
    TemplateDefNode *node = new TemplateDefNode();
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

TemplateDefNode *TemplateDefNode::setModifiers(ModifiersNode *modifiers) {
    if (classDef) {
        classDef->setModifiers(modifiers);
    }

    return this;
}

TemplateDefNode *TemplateDefNode::copy() {
    TemplateDefNode *node = new TemplateDefNode();
    node->type = type;

    if (classDef) {
        node->classDef = classDef->copy();
    }

    return node;
}

ModifiersNode *TemplateDefNode::getModifiers() const {
    if (classDef) {
        return classDef->modifiers;
    }

    return nullptr;
}

std::list<Node *> TemplateDefNode::getChildren() const {
    std::list<Node *> children = {};
    addChildIfNotNull(children, classDef);

    return children;
}
