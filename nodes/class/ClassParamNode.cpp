#include "ClassParamNode.h"

ClassParamNode::ClassParamNode() {
    modifiers = nullptr;
    fullId = nullptr;
    compoundType = nullptr;
}

ClassParamNode *ClassParamNode::createClassParam(ClassParamType type, ModifiersNode *modifiers, IdNode *fullId,
                                                 CompoundTypeNode *compoundType) {
    ClassParamNode* node = new ClassParamNode();
    node->type = type;
    node->modifiers = modifiers;
    node->fullId = fullId;
    node->compoundType = compoundType;
    return node;
}

string ClassParamNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, modifiers, "modifiers");
    addDotChild(dot, fullId, "fullId");
    addDotChild(dot, compoundType, "Compound type");

    return dot;
}

string ClassParamNode::getDotLabel() const {
    return classParamTypeToString(type);
}
