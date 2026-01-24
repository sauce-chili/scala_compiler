#include "DclNode.h"

DclNode::DclNode() {
    fullId = nullptr;
    simpleType = nullptr;
    funSig = nullptr;
    modifiers = nullptr;
}

DclNode *DclNode::createVarDcl(IdNode *fullId, SimpleTypeNode* simpleType) {
    DclNode* node = new DclNode();
    node->type = _VAR_DECL;
    node->fullId = fullId;
    node->simpleType = simpleType;
    return node;
}

DclNode *DclNode::createValDcl(IdNode *fullId, SimpleTypeNode* simpleType) {
    DclNode* node = new DclNode();
    node->type = _VAL_DECL;
    node->fullId = fullId;
    node->simpleType = simpleType;
    return node;
}

DclNode *DclNode::createDefDcl(FunSigNode *funSig, SimpleTypeNode* simpleType) {
    DclNode* node = new DclNode();
    node->type = _FUN_DEF;
    node->funSig = funSig;
    node->simpleType = simpleType;
    return node;
}


DclNode *DclNode::setModifiers(ModifiersNode *modifiers) {
    this->modifiers = modifiers->copy();
    return this;
}

DclNode *DclNode::copy() {
    DclNode* node = new DclNode();

    node->type = type;

    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (simpleType) {
        node->simpleType = simpleType->copy();
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

    return fullId->name == name;
}

string DclNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "ids_");
    addDotChild(dot, funSig, "funSig_");
    addDotChild(dot, simpleType, "simpleType_");

    return dot;
}

string DclNode::getDotLabel() const {
    return "Declaration " + statTypeToString(type);
}

list<Node *> DclNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, modifiers);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, simpleType);
    addChildIfNotNull(children, funSig);
    return children;
}

