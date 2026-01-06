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

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ARGUMENTEXPRSNODE_H
