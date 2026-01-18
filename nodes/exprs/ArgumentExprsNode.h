#ifndef SCALA_LEXER_ARGUMENTEXPRSNODE_H
#define SCALA_LEXER_ARGUMENTEXPRSNODE_H

#include "../Node.h"
#include "ExprNode.h"
#include "ExprsNode.h"
#include <list>

class ArgumentExprsNode: public Node {
public:
    ExprsNode* exprs;

    ArgumentExprsNode(ExprsNode* exprs);

    ArgumentExprsNode* copy();

    bool contains(string name);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_ARGUMENTEXPRSNODE_H
