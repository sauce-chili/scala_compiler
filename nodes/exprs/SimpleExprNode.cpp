#include "SimpleExprNode.h"
#include "../class/ConstrInvokeNode.h"
#include "../stats/BlockStatsNode.h"
#include "SimpleExpr1Node.h"

SimpleExprNode::SimpleExprNode() {
    fullId = nullptr;
    arguments = nullptr;
    simpleType = nullptr;
    blockStats = nullptr;
    simpleExpr1 = nullptr;
}

SimpleExprNode *SimpleExprNode::createNewObjectNode(IdNode *fullId, ArgumentExprsNode *arguments) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _INSTANCE_CREATING;
    node->fullId = fullId;
    node->arguments = arguments;
    return node;
}

SimpleExprNode *SimpleExprNode::createNewArrayNode(SimpleTypeNode *simpleType, ArgumentExprsNode *arguments) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _ARRAY_CREATING;
    node->simpleType = simpleType;
    node->arguments = arguments;
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
    addDotChild(dot, fullId, "new object's type");
    addDotChild(dot, arguments, "arguments");
    addDotChild(dot, simpleType, "type of array");
    addDotChild(dot, blockStats, "block statements");
    addDotChild(dot, simpleExpr1, "simple expr 1");

    return dot;
}

string SimpleExprNode::getDotLabel() const {
    return simpleExprToString(type);
}
