#include "EnumDefNode.h"

EnumDefNode::EnumDefNode() {
    fullId = nullptr;
    modifier = nullptr;
    classParams = nullptr;
    enumTemplate = nullptr;
}

EnumDefNode *EnumDefNode::createWithAccessModifier(IdNode *fullId, ModifierNode *modifier, ClassParamsNode *classParams,
                                                   EnumTemplateNode *enumTemplate) {
    EnumDefNode* node = new EnumDefNode();
    node->fullId = fullId;
    node->modifier = modifier;
    node->classParams = classParams;
    node->enumTemplate = enumTemplate;
    return node;
}

EnumDefNode *EnumDefNode::createWithClassParams(IdNode *fullId, ClassParamsNode *classParams, EnumTemplateNode *enumTemplate) {
    EnumDefNode* node = new EnumDefNode();
    node->fullId = fullId;
    node->classParams = classParams;
    node->enumTemplate = enumTemplate;
    return node;
}

EnumDefNode *EnumDefNode::createEnumTemplate(IdNode *fullId, EnumTemplateNode *enumTemplate) {
    EnumDefNode* node = new EnumDefNode();
    node->fullId = fullId;
    node->enumTemplate = enumTemplate;
    return node;
}

string EnumDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, modifier, "modifier_");
    addDotChild(dot, classParams, "classParams_");
    addDotChild(dot, enumTemplate, "enumTemplate_");

    return dot;
}

string EnumDefNode::getDotLabel() const {
    return "Enum definition";
}

list<Node *> EnumDefNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, modifier);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, classParams);
    addChildIfNotNull(children, enumTemplate);
    return children;
}

