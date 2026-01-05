#ifndef SCALA_LEXER_TRYEXPRNODE_H
#define SCALA_LEXER_TRYEXPRNODE_H

#include "../Node.h"
#include "../exprs/ExprNode.h"

class TryExprNode: public Node {
public:
    ExprNode* tryExpr;
    ExprNode* catchExpr;
    ExprNode* finallyNode;

    TryExprNode();

    static TryExprNode* createExceptionBlock(ExprNode* tryExpr, ExprNode* catchExpr, ExprNode* finallyNode);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TRYEXPRNODE_H
