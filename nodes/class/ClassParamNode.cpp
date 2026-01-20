#include "ClassParamNode.h"

ClassParamNode::ClassParamNode() {
    modifiers = nullptr;
    fullId = nullptr;
    simpleType = nullptr;
}

ClassParamNode *ClassParamNode::createClassParam(ClassParamType type, ModifiersNode *modifiers, IdNode *fullId,
                                                 SimpleTypeNode* simpleType) {
    ClassParamNode* node = new ClassParamNode();
    node->type = type;
    node->modifiers = modifiers;
    node->fullId = fullId;
    node->simpleType = simpleType;
    return node;
}

string ClassParamNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, modifiers, "modifiers");
    addDotChild(dot, fullId, "fullId");
    addDotChild(dot, simpleType, "Simple type");

    return dot;
}

string ClassParamNode::getDotLabel() const {
    return classParamTypeToString(type);
}
