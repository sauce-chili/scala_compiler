#include "ConstrExprNode.h"
#include "SimpleExpr1Node.h"
#include "../id/IdNode.h"

ConstrExprNode::ConstrExprNode() {
    blockStats = nullptr;
}

ConstrExprNode *ConstrExprNode::createConstrExpr(ArgumentExprsNode *argumentExprs, BlockStatsNode *blockStats, bool isBlock) {
    ConstrExprNode* node = new ConstrExprNode();
    BlockStatNode* blockStat = BlockStatNode::createSimpleExpr1(SimpleExpr1Node::createMethodCallNode(
            SimpleExpr1Node::createIdNode(IdNode::createId("this")),
            argumentExprs
            ));
    blockStats = BlockStatsNode::addBlockStatFrontToList(blockStats, blockStat);
    node->blockStats = blockStats;
    node->isBlock = isBlock;
    return node;
}

ConstrExprNode *ConstrExprNode::copy() {
    ConstrExprNode* node = new ConstrExprNode();
    node->isBlock = isBlock;

    if (blockStats) {
        node->blockStats = blockStats->copy();
    }
    return node;
}

string ConstrExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, blockStats, "blockStats_");

    return dot;
}

string ConstrExprNode::getDotLabel() const {
    return "Constructor expr";
}

list<Node *> ConstrExprNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, blockStats);
    return children;
}

