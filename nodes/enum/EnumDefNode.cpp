//
// Created by Roman on 1/6/2026.
//

#include "EnumDefNode.h"

EnumDefNode::EnumDefNode() {

}

EnumDefNode *EnumDefNode::createWithAccessModifier(IdNode *id, ModifierNode *modifier, ClassParamsNode *classParams,
                                                   EnumTemplateNode *enumTemplate) {
    EnumDefNode* node = new EnumDefNode();
    node->id = id;
    node->modifier = modifier;
    node->classParams = classParams;
    node->enumTemplate = enumTemplate;
    return node;
}

EnumDefNode *EnumDefNode::createWithClassParams(IdNode *id, ClassParamsNode *classParams, EnumTemplateNode *enumTemplate) {
    EnumDefNode* node = new EnumDefNode();
    node->id = id;
    node->classParams = classParams;
    node->enumTemplate = enumTemplate;
    return node;
}

EnumDefNode *EnumDefNode::createEnumTemplate(IdNode *id, EnumTemplateNode *enumTemplate) {
    EnumDefNode* node = new EnumDefNode();
    node->id = id;
    node->enumTemplate = enumTemplate;
    return node;
}

string EnumDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, id, "id_");
    addDotChild(dot, modifier, "modifier_");
    addDotChild(dot, classParams, "classParams_");
    addDotChild(dot, enumTemplate, "enumTemplate_");

    return dot;
}

string EnumDefNode::getDotLabel() const {
    return "Enum definition";
}
