#include "ClassParamNode.h"

ClassParamNode::ClassParamNode() {

}

ClassParamNode *ClassParamNode::createClassParam(ClassParamType type, ModifiersNode *modifiers, IdNode *id,
                                                 CompoundTypeNode *compoundType, AssignExprNode *assignExpr) {
    ClassParamNode* node = new ClassParamNode();
    node->type = type;
    node->modifiers = modifiers;
    node->id = id;
    node->compoundType = compoundType;
    node->assignExpr = assignExpr;
}

string ClassParamNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, modifiers, "modifiers");
    addDotChild(dot, id, "id");
    addDotChild(dot, compoundType, "Compound type");
    addDotChild(dot, assignExpr, "Assign expr");

    return dot;
}

string ClassParamNode::getDotLabel() const {
    return classParamTypeToString(type);
}
