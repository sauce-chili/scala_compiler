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

    static ExprsNode* addExprToList(ExprsNode *exprs, ExprNode *expr);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_EXPRSNODE_H
