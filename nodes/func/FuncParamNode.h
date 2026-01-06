#ifndef SCALA_LEXER_FUNCPARAMNODE_H
#define SCALA_LEXER_FUNCPARAMNODE_H


#include "../Node.h"
#include "nodes/id/IdNode.h"
#include "nodes/type/CompoundTypeNode.h"
#include "nodes/exprs/AssignExprNode.h"

class FuncParamNode: public Node {
public:
    IdNode* fullId;
    CompoundTypeNode* compoundType;
    AssignExprNode* assignExpr;

    FuncParamNode();

    static FuncParamNode* createClassParam(IdNode* fullId, CompoundTypeNode* compoundType, AssignExprNode* assignExpr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_FUNCPARAMNODE_H
