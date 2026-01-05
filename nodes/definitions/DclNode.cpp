//
// Created by Roman on 1/5/2026.
//

#include "DclNode.h"

DclNode::DclNode() {

}

DclNode *DclNode::createVarDcl(IdsNode *ids, CompoundTypeNode *compoundType) {
    DclNode* node = new DclNode();
    node->type = VAR_DECL;
    node->ids = ids;
    node->compoundType = compoundType;
    return node;
}

DclNode *DclNode::createValDcl(IdsNode *ids, CompoundTypeNode *compoundType) {
    DclNode* node = new DclNode();
    node->type = VAL_DECL;
    node->ids = ids;
    node->compoundType = compoundType;
    return node;
}

DclNode *DclNode::createDefDcl(FunSigNode *funSig, CompoundTypeNode *compoundType) {
    DclNode* node = new DclNode();
    node->type = FUN_DEF;
    node->funSig = funSig;
    node->compoundType = compoundType;
    return node;
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
    return "Declaration";
}
