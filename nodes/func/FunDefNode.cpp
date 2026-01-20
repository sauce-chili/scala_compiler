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
    FunDefNode* node = new FunDefNode();
    node->funcParams = funcParams;
    node->constrExpr = constrExpr;
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
