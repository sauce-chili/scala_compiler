#include "ClassDefNode.h"

ClassDefNode::ClassDefNode() {
    fullId = nullptr;
    primaryConstructModifier = nullptr;
    classParams = nullptr;
    classTemplateOpt = nullptr;
}

ClassDefNode *ClassDefNode::createClassDef(IdNode *fullId, ModifierNode *modifier, ClassParamsNode *classParams,
                                           ClassTemplateOptNode *classTemplateOpt) {
    ClassDefNode* node = new ClassDefNode();
    node->fullId = fullId;
    if (!modifier) {
        modifier = ModifierNode::createModifier(_PUBLIC);
    }
    node->primaryConstructModifier = modifier;
    node->classParams = classParams;
    node->classTemplateOpt = classTemplateOpt;
    return node;
}

string ClassDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "id_");
    addDotChild(dot, primaryConstructModifier, "primary_constructor_modifier_");
    addDotChild(dot, classParams, "classParams_");
    addDotChild(dot, classTemplateOpt, "classTemplateOpt_");

    return dot;
}

string ClassDefNode::getDotLabel() const {
    return "Class definition";
}
