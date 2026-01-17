#ifndef SCALA_LEXER_VARDEFSNODE_H
#define SCALA_LEXER_VARDEFSNODE_H


#include "../Node.h"
#include "../Types.h."
#include "../id/IdsNode.h"
#include "../type/CompoundTypeNode.h"
#include "../exprs/ExprNode.h"


class VarDefsNode: public Node {
public:
    IdsNode* ids;
    CompoundTypeNode* compoundType;
    ExprNode* expr;
    StatType type;

    VarDefsNode();

    static VarDefsNode* createVal(IdsNode* ids, CompoundTypeNode* compoundType, ExprNode* expr);
    static VarDefsNode* createVar(IdsNode* ids, CompoundTypeNode* compoundType, ExprNode* expr);

    VarDefsNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_VARDEFSNODE_H
