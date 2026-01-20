#include "DclNode.h"

DclNode::DclNode() {
    ids = nullptr;
    simpleType = nullptr;
    funSig = nullptr;
}

DclNode *DclNode::createVarDcl(IdsNode *ids, SimpleTypeNode* simpleType) {
    DclNode* node = new DclNode();
    node->type = _VAR_DECL;
    node->ids = ids;
    node->simpleType = simpleType;
    return node;
}

DclNode *DclNode::createValDcl(IdsNode *ids, SimpleTypeNode* simpleType) {
    DclNode* node = new DclNode();
    node->type = _VAL_DECL;
    node->ids = ids;
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

string DclNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, ids, "ids_");
    addDotChild(dot, funSig, "funSig_");
    addDotChild(dot, simpleType, "simpleType_");

    return dot;
}

string DclNode::getDotLabel() const {
    return "Declaration";
}
