#include "TryExprNode.h"

TryExprNode::TryExprNode() {
    tryExpr = nullptr;
    catchExpr = nullptr;
    finallyNode = nullptr;
}

TryExprNode *TryExprNode::createExceptionBlock(ExprNode *tryExpr, ExprNode *catchExpr, ExprNode *finallyNode) {
    TryExprNode* node = new TryExprNode();
    node->tryExpr = tryExpr;
    node->catchExpr = catchExpr;
    node->finallyNode = finallyNode;
    return node;
}

TryExprNode *TryExprNode::copy() {
    TryExprNode* node = new TryExprNode();
    if (tryExpr) {
        node->tryExpr = tryExpr->copy();
    }
    if (catchExpr) {
        node->catchExpr = catchExpr->copy();
    }
    if (finallyNode) {
        node->finallyNode = finallyNode->copy();
    }
    return node;
}

string TryExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, tryExpr, "Try expr");
    addDotChild(dot, catchExpr, "Catch expr");
    addDotChild(dot, finallyNode, "Finally expr");

    return dot;
}

string TryExprNode::getDotLabel() const {
    return "Exception block";
}
