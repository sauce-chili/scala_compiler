#ifndef SCALA_LEXER_SUPERCONSTRUCTORCALLNODE_H
#define SCALA_LEXER_SUPERCONSTRUCTORCALLNODE_H

#include "../Node.h"
#include "../exprs/ArgumentExprsNode.h"

class SuperConstructorCallNode: public Node {
public:
    ArgumentExprsNode* argumentExprs;

    SuperConstructorCallNode();
    SuperConstructorCallNode(ArgumentExprsNode* argumentExprs);

    SuperConstructorCallNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_SUPERCONSTRUCTORCALLNODE_H
