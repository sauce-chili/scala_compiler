#include "EnumStatNode.h"

EnumStatNode::EnumStatNode() {
    templateStat = nullptr;
    modifiers = nullptr;
    enumCase = nullptr;
}

EnumStatNode *EnumStatNode::createWithTemplateStat(TemplateStatNode *templateStat) {
    EnumStatNode* node = new EnumStatNode();
    node->templateStat = templateStat;
    return node;
}

EnumStatNode *EnumStatNode::createWithEnumCase(ModifiersNode *modifiers, EnumCaseNode *enumCase) {
    EnumStatNode* node = new EnumStatNode();
    node->modifiers = modifiers;
    node->enumCase = enumCase;
    return node;
}

EnumStatNode *EnumStatNode::copy() {
    EnumStatNode* node = new EnumStatNode();

    if (templateStat) {
        node->templateStat = templateStat->copy();
    }
    if (modifiers) {
        node->modifiers = modifiers->copy();
    }
    if (enumCase) {
        node->enumCase = enumCase->copy();
    }

    return node;
}

string EnumStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, modifiers, "modifiers_");
    addDotChild(dot, enumCase, "enumCase_");

    return dot;
}

string EnumStatNode::getDotLabel() const {
    return "Enum stat";
}
