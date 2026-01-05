#include "FunSigNode.h"

FunSigNode::FunSigNode() {

}

FunSigNode *FunSigNode::createFunSig(IdNode *id, FuncParamsNode *params) {
    FunSigNode* node = new FunSigNode();
    node->id = id;
    node->params = params;
}

string FunSigNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, id, "id_");
    addDotChild(dot, params, "params_");

    return dot;
}

string FunSigNode::getDotLabel() const {
    return "Fun signature";
}
