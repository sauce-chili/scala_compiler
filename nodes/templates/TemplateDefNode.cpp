#include "TemplateDefNode.h"
#include "../class/ClassDefNode.h"
#include "../trait/TraitTemplateOptNode.h."
#include "../enum/EnumDefNode.h"

TemplateDefNode::TemplateDefNode() {

}

TemplateDefNode *TemplateDefNode::createClassDef(ClassDefNode *classDef) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = _CLASS;
    node->classDef = classDef;
    return node;
}

TemplateDefNode *TemplateDefNode::createObjectDef(IdNode *fullId, ClassTemplateOptNode *classTemplateOpt) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = _TRAIT;
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

string TemplateDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, classDef, "classDef_");
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, classTemplateOpt, "classTemplateOpt_");
    addDotChild(dot, traitTemplateOpt, "traitTemplateOpt_");
    addDotChild(dot, enumDef, "enumDef_");

    return dot;
}

string TemplateDefNode::getDotLabel() const {
    return "Template definition of " + statTypeToString(type);
}
