#include "ConstrExprNode.h"

ConstrExprNode::ConstrExprNode() {

}

ConstrExprNode *ConstrExprNode::createConstrExpr(ArgumentExprsNode *argumentExprs, BlockStatsNode *blockStats) {
    ConstrExprNode* node = new ConstrExprNode();
    node->argumentExprs = argumentExprs;
    node->blockStats = blockStats;
    return node;
}

string ConstrExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, argumentExprs, "argumentExprs_");
    addDotChild(dot, blockStats, "blockStats_");

    return dot;
}

string ConstrExprNode::getDotLabel() const {
    return "Constructor expr";
}
