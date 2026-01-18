#include "DclNode.h"

DclNode::DclNode() {
    ids = nullptr;
    compoundType = nullptr;
    funSig = nullptr;
    modifiers = nullptr;
}

DclNode *DclNode::createVarDcl(IdsNode *ids, CompoundTypeNode *compoundType) {
    DclNode* node = new DclNode();
    node->type = _VAR_DECL;
    node->ids = ids;
    node->compoundType = compoundType;
    return node;
}

DclNode *DclNode::createValDcl(IdsNode *ids, CompoundTypeNode *compoundType) {
    DclNode* node = new DclNode();
    node->type = _VAL_DECL;
    node->ids = ids;
    node->compoundType = compoundType;
    return node;
}

DclNode *DclNode::createDefDcl(FunSigNode *funSig, CompoundTypeNode *compoundType) {
    DclNode* node = new DclNode();
    node->type = _FUN_DEF;
    node->funSig = funSig;
    node->compoundType = compoundType;
    return node;
}

DclNode *DclNode::setModifiers(ModifiersNode *modifiers) {
    this->modifiers = modifiers;
    return this;
}

DclNode *DclNode::copy() {
    DclNode* node = new DclNode();

    node->type = type;

    if (ids) {
        node->ids = ids->copy();
    }
    if (compoundType) {
        node->compoundType = compoundType->copy();
    }
    if (funSig) {
        node->funSig = funSig->copy();
    }
    if (modifiers) {
        node->modifiers = modifiers->copy();
    }

    return node;
}

bool DclNode::containsVar(string name) {

    if (type != _VAL_DECL && type != _VAR_DECL) return false;

    return ids->contains(name);
}

string DclNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, ids, "ids_");
    addDotChild(dot, funSig, "funSig_");
    addDotChild(dot, compoundType, "compoundType_");

    return dot;
}

string DclNode::getDotLabel() const {
    return "Declaration " + statTypeToString(type);
}

