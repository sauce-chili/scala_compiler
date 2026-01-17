#include "FunSigNode.h"

FunSigNode::FunSigNode() {
    fullId = nullptr;
    params = nullptr;
}

FunSigNode *FunSigNode::createFunSig(IdNode *fullId, FuncParamsNode *params) {
    FunSigNode* node = new FunSigNode();
    node->fullId = fullId;
    node->params = params;
    return node;
}

FunSigNode *FunSigNode::copy() {
    FunSigNode* node = new FunSigNode();

    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (params) {
        node->params = params->copy();
    }

    return node;
}

string FunSigNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, params, "params_");

    return dot;
}

string FunSigNode::getDotLabel() const {
    return "Fun signature";
}
