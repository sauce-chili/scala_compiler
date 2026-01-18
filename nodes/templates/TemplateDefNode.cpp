#include "TemplateDefNode.h"
#include "../class/ClassDefNode.h"
#include "../trait/TraitTemplateOptNode.h."
#include "../enum/EnumDefNode.h"

TemplateDefNode::TemplateDefNode() {
    classDef = nullptr;
    fullId = nullptr;
    classTemplateOpt = nullptr;
    traitTemplateOpt = nullptr;
    enumDef = nullptr;
    modifiers = nullptr;
}

TemplateDefNode *TemplateDefNode::createClassDef(ClassDefNode *classDef) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = _CLASS;
    node->classDef = classDef;
    return node;
}

TemplateDefNode *TemplateDefNode::createObjectDef(IdNode *fullId, ClassTemplateOptNode *classTemplateOpt) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = _OBJECT;
    node->fullId = fullId;
    node->classTemplateOpt = classTemplateOpt;
    return node;
}

TemplateDefNode *TemplateDefNode::createTraitDef(IdNode *fullId, TraitTemplateOptNode* traitTemplateOpt) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = _TRAIT;
    node->fullId = fullId;
    node->traitTemplateOpt = traitTemplateOpt;
    return node;
}

TemplateDefNode *TemplateDefNode::createEnumDef(EnumDefNode *enumDef) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = _ENUM;
    node->enumDef = enumDef;
    return node;
}

TemplateDefNode *TemplateDefNode::copy() {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = type;

    if (classDef) {
        node->classDef = classDef->copy();
    }
    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (classTemplateOpt) {
        node->classTemplateOpt = classTemplateOpt->copy();
    }
    if (traitTemplateOpt) {
        node->traitTemplateOpt = traitTemplateOpt->copy();
    }
    if (enumDef) {
        node->enumDef = enumDef->copy();
    }

    return node;
}

string TemplateDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, classDef, "classDef_");
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, classTemplateOpt, "classTemplateOpt_");
    addDotChild(dot, traitTemplateOpt, "traitTemplateOpt_");
    addDotChild(dot, enumDef, "enumDef_");
    addDotChild(dot, modifiers, "modifiers_");

    return dot;
}

string TemplateDefNode::getDotLabel() const {
    return "Template definition of " + statTypeToString(type);
}

TemplateDefNode * TemplateDefNode::setModifiers(ModifiersNode *modifiers) {
    this->modifiers = modifiers;
    return this;
}


std::list<Node*> TemplateDefNode::getChildren() const {
    std::list<Node*> children;
    addChildIfNotNull(children, classDef);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, classTemplateOpt);
    addChildIfNotNull(children, traitTemplateOpt);
    addChildIfNotNull(children, enumDef);
    addChildIfNotNull(children, modifiers);
    return children;
}
