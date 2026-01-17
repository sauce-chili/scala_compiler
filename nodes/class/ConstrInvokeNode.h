#ifndef SCALA_LEXER_CONSTRINVOKENODE_H
#define SCALA_LEXER_CONSTRINVOKENODE_H

#include "../Node.h"
#include "../type/SimpleTypeNode.h"
#include "../exprs/ArgumentExprsNode.h"

class ConstrInvokeNode: public Node {
public:
    StableIdNode* stableId;
    ArgumentExprsNode* arguments;

    ConstrInvokeNode();

    ConstrInvokeNode* copy();

    static ConstrInvokeNode* createConstrInvokeNode(StableIdNode* stableId, ArgumentExprsNode* arguments);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CONSTRINVOKENODE_H
