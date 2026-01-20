#ifndef SCALA_LEXER_VARDEFSNODE_H
#define SCALA_LEXER_VARDEFSNODE_H


#include "../Node.h"
#include "../Types.h."
#include "../type/SimpleTypeNode.h"
#include "../id/IdsNode.h"
#include "../exprs/ExprNode.h"


class VarDefsNode: public Node {
public:
    IdsNode* ids;
    SimpleTypeNode* simpleType;
    ExprNode* expr;
    StatType type;

    VarDefsNode();

    static VarDefsNode* createVal(IdsNode* ids, SimpleTypeNode* simpleType, ExprNode* expr);
    static VarDefsNode* createVar(IdsNode* ids, SimpleTypeNode* simpleType, ExprNode* expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_VARDEFSNODE_H
