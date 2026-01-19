#include "ClassParamNode.h"

ClassParamNode::ClassParamNode() {
    modifiers = nullptr;
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

ClassParamNode* ClassParamNode::copy() {
    ClassParamNode* node = new ClassParamNode();
    node->type = type;
    if (modifiers) {
        node->modifiers = modifiers->copy();
    }
    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (compoundType) {
        node->compoundType = compoundType->copy();
    }
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

list<Node *> ClassParamNode::getChildren() const {
    std::list<Node *> children = {};
    addChildIfNotNull(children, modifiers);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, compoundType);
    return children;
}
