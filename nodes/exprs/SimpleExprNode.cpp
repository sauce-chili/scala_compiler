#include "SimpleExprNode.h"
#include "../class/ConstrInvokeNode.h"
#include "../stats/BlockStatsNode.h"
#include "SimpleExpr1Node.h"

SimpleExprNode::SimpleExprNode() {
    constr = nullptr;
    blockStats = nullptr;
    simpleExpr1 = nullptr;
}

SimpleExprNode* SimpleExprNode::createConstrInvokeNode(ConstrInvokeNode* constr) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _INSTANCE_CREATING;
    node->constr = constr;
    return node;
}

SimpleExprNode* SimpleExprNode::createBlockStatsNode(BlockStatsNode* blockStats) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _BLOCK_STATS;
    node->blockStats = blockStats;
    return node;
}

SimpleExprNode* SimpleExprNode::createSimpleExpr1Node(SimpleExpr1Node* simpleExpr1) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _SIMPLE_EXPR_1;
    node->simpleExpr1 = simpleExpr1;
    return node;
}

string SimpleExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, constr, "constr invoke");
    addDotChild(dot, blockStats, "block statements");
    addDotChild(dot, simpleExpr1, "simple expr 1");

    return dot;
}

string SimpleExprNode::getDotLabel() const {
    return simpleExprToString(type);
}
