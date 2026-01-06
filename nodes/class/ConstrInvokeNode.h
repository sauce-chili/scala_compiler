#ifndef SCALA_LEXER_CONSTRINVOKENODE_H
#define SCALA_LEXER_CONSTRINVOKENODE_H

#include "../Node.h"
#include "../type/SimpleTypeNode.h"
#include "../exprs/ArgumentExprsNode.h"

class ConstrInvokeNode: public Node {
public:
    SimpleTypeNode* simpleType;
    ArgumentExprsNode* arguments;

    ConstrInvokeNode();

    static ConstrInvokeNode* createWithArgumentsNode(SimpleTypeNode* simpleType, ArgumentExprsNode* arguments);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CONSTRINVOKENODE_H
