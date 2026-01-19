#include "PrimaryConstructorNode.h"

PrimaryConstructorNode::PrimaryConstructorNode() {

}

PrimaryConstructorNode::PrimaryConstructorNode(FuncParamsNode *funcParams, BlockStatsNode *blockStats, SuperConstructorCallNode *superCall) {
    this->funcParams = funcParams;
    this->blockStats = blockStats;
    this->superCall = superCall;
}

PrimaryConstructorNode *PrimaryConstructorNode::copy() {
    PrimaryConstructorNode* node = new PrimaryConstructorNode();
    if (funcParams) {
        node->funcParams = funcParams->copy();
    }
    if (blockStats) {
        node->blockStats = blockStats->copy();
    }
    if (superCall) {
        node->superCall = superCall->copy();
    }
    return node;
}

string PrimaryConstructorNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, funcParams, "funcParams_");
    addDotChild(dot, blockStats, "blockStats_");
    addDotChild(dot, superCall, "superCall_");

    return dot;
}

string PrimaryConstructorNode::getDotLabel() const {
    return "Primary constructor";
}
