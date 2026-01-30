#ifndef SCALA_LEXER_FUNDEFNODE_H
#define SCALA_LEXER_FUNDEFNODE_H

#include "../Node.h"
#include "FunSigNode.h"
#include "../exprs/ConstrExprNode.h"

class FunDefNode: public Node {
public:
    FunSigNode* funSig;
    SimpleTypeNode* simpleType;
    ConstrExprNode* constrExpr;
    ExprNode* expr;
    FuncParamsNode* funcParams;
    bool primaryConstructor = false;

    FunDefNode();

    static FunDefNode* createFunSigFunDef(FunSigNode* funSig, SimpleTypeNode* simpleType, ExprNode* expr);
    static FunDefNode* createThisConstrCallFunDef(FuncParamsNode* funcParams, ConstrExprNode* constrExpr);

    ExprNode* getBody() const;

    FunDefNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    bool isConstructor();
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_FUNDEFNODE_H
