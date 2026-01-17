#include "PrimaryConstructorNode.h"

PrimaryConstructorNode::PrimaryConstructorNode(FuncParamsNode *funcParams, BlockStatsNode *blockStats, SuperConstructorCallNode *superCall) {
    this->funcParams = funcParams;
    this->blockStats = blockStats;
    this->superCall = superCall;
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
