#include "FunDefNode.h"

FunDefNode::FunDefNode() {
    funSig = nullptr;
    compoundType = nullptr;
    constrExpr = nullptr;
    expr = nullptr;
    funcParams = nullptr;
}

FunDefNode *FunDefNode::createFunSigFunDef(FunSigNode *funSig, CompoundTypeNode *compoundType, ExprNode *expr) {
    FunDefNode *node = new FunDefNode();
    node->funSig = funSig;
    node->compoundType = compoundType;
    node->expr = expr;
    return node;
}

FunDefNode *FunDefNode::createThisConstrCallFunDef(FuncParamsNode *funcParams, ConstrExprNode *constrExpr) {
    FunDefNode *node = new FunDefNode();
    node->funcParams = funcParams;
    node->constrExpr = constrExpr;
    return node;
}

FunDefNode *FunDefNode::copy() {
    FunDefNode* node = new FunDefNode();

    if (funSig) {
        node->funSig = funSig->copy();
    }
    if (compoundType) {
        node->compoundType = compoundType->copy();
    }
    if (constrExpr) {
        node->constrExpr = constrExpr->copy();
    }
    if (expr) {
        node->expr = expr->copy();
    }
    if (funcParams) {
        node->funcParams = funcParams->copy();
    }

    return node;
}

string FunDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, funSig, "funSig_");
    addDotChild(dot, compoundType, "compoundType_");
    addDotChild(dot, expr, "expr_");
    addDotChild(dot, funcParams, "funcParams_");
    addDotChild(dot, constrExpr, "constrExpr_");

    return dot;
}

string FunDefNode::getDotLabel() const {
    return "Fun def";
}

bool FunDefNode::isConstructor() {
    return nullptr != constrExpr;
}

list<Node *> FunDefNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, funSig);
    addChildIfNotNull(children, funcParams);
    addChildIfNotNull(children, compoundType);
    addChildIfNotNull(children, expr);
    addChildIfNotNull(children, constrExpr);
    return children;
}
