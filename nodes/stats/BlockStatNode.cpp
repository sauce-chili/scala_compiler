#include "BlockStatNode.h"
#include "../var/VarDefsNode.h"
#include "../exprs/InfixExprNode.h"

BlockStatNode::BlockStatNode() {
    varDefs = nullptr;
    expr = nullptr;
}

BlockStatNode *BlockStatNode::createVarDefsNode(VarDefsNode *varDefs) {
    BlockStatNode* node = new BlockStatNode();
    node->varDefs = varDefs;
    return node;
}

BlockStatNode *BlockStatNode::createExprNode(ExprNode *expr) {
    BlockStatNode* node = new BlockStatNode();
    node->expr = expr;
    return node;
}

BlockStatNode *BlockStatNode::copy() {
    BlockStatNode* node = new BlockStatNode();

    if (varDefs) {
        node->varDefs = varDefs->copy();
    }
    if (expr) {
        node->expr = expr->copy();
    }
    return node;
}

string BlockStatNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, varDefs, "Var defs");
    addDotChild(dot, expr, "Expr");

    return dot;
}

string BlockStatNode::getDotLabel() const {
    return "Block stat";
}

list<Node *> BlockStatNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, varDefs);
    addChildIfNotNull(children, expr);
    return children;
}

BlockStatNode *BlockStatNode::createSimpleExpr1(SimpleExpr1Node* simpleExpr1) {
    InfixExprNode* infix = InfixExprNode::createInfixFromPrefix(
            PrefixExprNode::createPrefixExprNode(
                    SimpleExprNode::createSimpleExpr1Node(
                            simpleExpr1
                    )
                    , _NO_UNARY_OPERATOR )
    );

    return createExprNode(ExprNode::createInfix(infix));
}

ExprNode *BlockStatNode::createExpr(SimpleExprNode* simpleExpr) {
    InfixExprNode* infix = InfixExprNode::createInfixFromPrefix(
            PrefixExprNode::createPrefixExprNode(
                    simpleExpr
                    , _NO_UNARY_OPERATOR )
    );

    return createExprNode(ExprNode::createInfix(infix))->expr;
}

