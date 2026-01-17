#ifndef SCALA_LEXER_EXPRSNODE_H
#define SCALA_LEXER_EXPRSNODE_H

#include "../Node.h"
#include "ExprNode.h"
#include <list>

class ExprsNode: public Node {
public:
    std::list<ExprNode*> *exprs;

    ExprsNode(ExprNode *first);
    ExprsNode();

    ExprsNode* copy();

    static ExprsNode* addExprToList(ExprsNode *exprs, ExprNode *expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_EXPRSNODE_H
