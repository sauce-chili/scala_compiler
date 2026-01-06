#ifndef SCALA_LEXER_INFIXEXPRNODE_H
#define SCALA_LEXER_INFIXEXPRNODE_H


#include "../Node.h"
#include "../id/IdNode.h"
#include "../exprs/PrefixExprNode.h"

class InfixExprNode: public Node {
public:
    InfixExprNode *left, *right;
    IdNode* fullId;
    PrefixExprNode* prefixExpr;

    InfixExprNode();
    static InfixExprNode* createInfixFromPrefix(PrefixExprNode* prefixExpr);
    static InfixExprNode* createFromInfixes(InfixExprNode *left, IdNode* fullId, InfixExprNode *right);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_INFIXEXPRNODE_H
