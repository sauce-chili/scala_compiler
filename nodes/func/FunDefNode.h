#ifndef SCALA_LEXER_FUNDEFNODE_H
#define SCALA_LEXER_FUNDEFNODE_H

#include "../Node.h"
#include "FunSigNode.h"
#include "../exprs/ConstrExprNode.h"

class FunDefNode: public Node {
public:
    FunSigNode* funSig;
    CompoundTypeNode* compoundType;
    ConstrExprNode* constrExpr;
    ExprNode* expr;
    FuncParamsNode* funcParams;

    FunDefNode();

    static FunDefNode* createFunSigFunDef(FunSigNode* funSig, CompoundTypeNode* compoundType, ExprNode* expr);
    static FunDefNode* createThisConstrCallFunDef(FuncParamsNode* funcParams, ConstrExprNode* constrExpr);

    FunDefNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    bool isConstructor();
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_FUNDEFNODE_H
