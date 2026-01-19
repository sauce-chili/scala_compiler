#include "ConstrExprNode.h"

ConstrExprNode::ConstrExprNode() {
    argumentExprs = nullptr;
    blockStats = nullptr;
}

ConstrExprNode *ConstrExprNode::createConstrExpr(ArgumentExprsNode *argumentExprs, BlockStatsNode *blockStats, bool isBlock) {
    ConstrExprNode* node = new ConstrExprNode();
    node->argumentExprs = argumentExprs;
    node->blockStats = blockStats;
    node->isBlock = isBlock;
    return node;
}

ConstrExprNode *ConstrExprNode::copy() {
    ConstrExprNode* node = new ConstrExprNode();
    node->isBlock = isBlock;

    if (argumentExprs) {
        node->argumentExprs = argumentExprs->copy();
    }
    if (blockStats) {
        node->blockStats = blockStats->copy();
    }
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

list<Node *> ConstrExprNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, argumentExprs);
    addChildIfNotNull(children, blockStats);
    return children;
}

