#include "FunDefNode.h"

FunDefNode::FunDefNode() {

}

FunDefNode *FunDefNode::createFunSigFunDef(FunSigNode *funSig, CompoundTypeNode *compoundType, ExprNode *expr) {
    FunDefNode* node = new FunDefNode();
    node->funSig = funSig;
    node->compoundType = compoundType;
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
    addDotChild(dot, compoundType, "compoundType_");
    addDotChild(dot, expr, "expr_");
    addDotChild(dot, funcParams, "funcParams_");
    addDotChild(dot, constrExpr, "constrExpr_");

    return dot;
}

string FunDefNode::getDotLabel() const {
    return "Fun def";
}
