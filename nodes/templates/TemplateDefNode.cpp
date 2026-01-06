#include "TemplateDefNode.h"

TemplateDefNode::TemplateDefNode() {

}

TemplateDefNode *TemplateDefNode::createClassDef(ClassDefNode *classDef) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = CLASS;
    node->classDef = classDef;
    return node;
}

TemplateDefNode *TemplateDefNode::createObjectDef(IdNode *id, ClassTemplateOptNode *classTemplateOpt) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = TRAIT;
    node->id = id;
    node->classTemplateOpt = classTemplateOpt;
    return node;
}

TemplateDefNode *TemplateDefNode::createTraitDef(IdNode *id, TraitTemplateOptNode* traitTemplateOpt) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = TRAIT;
    node->id = id;
    node->traitTemplateOpt = traitTemplateOpt;
    return node;
}

TemplateDefNode *TemplateDefNode::createEnumDef(EnumDefNode *enumDef) {
    TemplateDefNode* node = new TemplateDefNode();
    node->type = ENUM;
    node->enumDef = enumDef;
    return node;
}

string TemplateDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, classDef, "classDef_");
    addDotChild(dot, id, "id_");
    addDotChild(dot, classTemplateOpt, "classTemplateOpt_");
    addDotChild(dot, traitTemplateOpt, "traitTemplateOpt_");
    addDotChild(dot, enumDef, "enumDef_");

    return dot;
}

string TemplateDefNode::getDotLabel() const {
    return "Template definition of " + statTypeToString(type);
}
