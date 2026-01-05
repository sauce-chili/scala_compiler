#ifndef SCALA_LEXER_ASSIGNEXPRNODE_H
#define SCALA_LEXER_ASSIGNEXPRNODE_H


#include "../Node.h"
#include "../exprs/ExprNode.h"

class AssignExprNode: public Node {
public:
    ExprNode* expr;

    AssignExprNode();

    static AssignExprNode* createAssignExprNode(ExprNode* expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ASSIGNEXPRNODE_H
