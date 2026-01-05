#include "SimpleExprNode.h"

SimpleExprNode::SimpleExprNode() {

}

SimpleExprNode* SimpleExprNode::clone() const {

}

SimpleExprNode* SimpleExprNode::createConstrInvokeNode(ConstrInvokeNode* constr) {
    SimpleExprNode* node = new SimpleExprNode();
    node->constr = constr;
    return node;
}

SimpleExprNode* SimpleExprNode::createBlockStatsNode(BlockStatsNode* blockStats) {
    SimpleExprNode* node = new SimpleExprNode();
    node->blockStats = blockStats;
    return node;
}

SimpleExprNode* SimpleExprNode::createSimpleExpr1Node(SimpleExpr1Node* simpleExpr1) {
    SimpleExprNode* node = new SimpleExprNode();
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
