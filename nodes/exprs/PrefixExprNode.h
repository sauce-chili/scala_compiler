#ifndef SCALA_LEXER_PREFIXEXPRNODE_H
#define SCALA_LEXER_PREFIXEXPRNODE_H


#include "../Node.h"
#include "SimpleExprNode.h"

class PrefixExprNode: public Node {
public:
    PrefixExprType type;
    SimpleExprNode* simpleExpr;

    PrefixExprNode();

    PrefixExprNode* copy();

    static PrefixExprNode* createPrefixExprNode(SimpleExprNode* simpleExpr, PrefixExprType type);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_PREFIXEXPRNODE_H
