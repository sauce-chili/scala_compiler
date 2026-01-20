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

TemplateDefNode * TemplateDefNode::setModifiers(ModifiersNode *modifiers) {
    if (classDef) {
        classDef->setModifiers(modifiers);
    } else if (classTemplateOpt) {
        classTemplateOpt->setModifiers(modifiers);
    } else if (traitTemplateOpt) {
        traitTemplateOpt->setModifiers(modifiers);
    } else if (enumDef) {
        enumDef->setModifiers(modifiers);
    }

    return this;
}

ModifiersNode * TemplateDefNode::getModifiers() const {
    if (classDef) {
        return classDef->modifiers;
    } else if (classTemplateOpt) {
        return classTemplateOpt->modifiers;
    } else if (traitTemplateOpt) {
        return traitTemplateOpt->modifiers;
    } else if (enumDef) {
        return enumDef->modifiers;
    }

    return nullptr;
}

std::list<Node*> TemplateDefNode::getChildren() const {
    std::list<Node*> children;
    addChildIfNotNull(children, classDef);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, classTemplateOpt);
    addChildIfNotNull(children, traitTemplateOpt);
    addChildIfNotNull(children, enumDef);
    return children;
}


