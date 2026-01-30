#include "FunDefNode.h"

FunDefNode::FunDefNode() {
    funSig = nullptr;
    simpleType = nullptr;
    constrExpr = nullptr;
    expr = nullptr;
    funcParams = nullptr;
}

FunDefNode *FunDefNode::createFunSigFunDef(FunSigNode *funSig, SimpleTypeNode* simpleType, ExprNode *expr) {
    FunDefNode* node = new FunDefNode();
    node->funSig = funSig;
    node->simpleType = simpleType;
    node->expr = expr;
    return node;
}

FunDefNode *FunDefNode::createThisConstrCallFunDef(FuncParamsNode *funcParams, ConstrExprNode *constrExpr) {
    FunDefNode *node = new FunDefNode();
    node->constrExpr = constrExpr;
    FunSigNode* constrSig = FunSigNode::createFunSig(IdNode::createId("this"), node->funcParams);
    node->funSig = constrSig;
    return node;
}

ExprNode* FunDefNode::getBody() const {
    if (constrExpr) {
        return ExprNode::createFromBlockStats(constrExpr->blockStats);
    }

    return expr;
}

FunDefNode *FunDefNode::copy() {
    FunDefNode* node = new FunDefNode();

    if (funSig) {
        node->funSig = funSig->copy();
    }
    if (simpleType) {
        node->simpleType = simpleType->copy();
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
    addDotChild(dot, simpleType, "simpleType_");
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
    addChildIfNotNull(children, simpleType);
    addChildIfNotNull(children, expr);
    addChildIfNotNull(children, constrExpr);
    return children;
}
