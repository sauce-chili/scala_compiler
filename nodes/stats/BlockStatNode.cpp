#include "BlockStatNode.h"
#include "../var/VarDefsNode.h"

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
    return "Block stats";
}

list<Node *> BlockStatNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, varDefs);
    addChildIfNotNull(children, expr);
    return children;
}

