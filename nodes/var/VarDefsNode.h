#ifndef SCALA_LEXER_VARDEFSNODE_H
#define SCALA_LEXER_VARDEFSNODE_H


#include "../Node.h"
#include "../Types.h."
#include "../type/SimpleTypeNode.h"
#include "../exprs/ExprNode.h"


class VarDefsNode: public Node {
public:
    IdNode* fullId;
    SimpleTypeNode* simpleType;
    ExprNode* expr;
    StatType type;

    VarDefsNode();

    static VarDefsNode* createVal(IdNode* fullId, SimpleTypeNode* simpleType, ExprNode* expr);
    static VarDefsNode* createVar(IdNode* fullId, SimpleTypeNode* simpleType, ExprNode* expr);

    VarDefsNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_VARDEFSNODE_H
