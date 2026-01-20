#ifndef SCALA_LEXER_SIMPLETYPENODE_H
#define SCALA_LEXER_SIMPLETYPENODE_H

#include "../Node.h"
#include "../id/IdNode.h"

class SimpleTypeNode: public Node {
public:
    IdNode* fullId;
    SimpleTypeNode* simpleType;

    SimpleTypeNode();

    static SimpleTypeNode* createIdTypeNode(IdNode* fullId);
    static SimpleTypeNode* createSimpleTypeNode(SimpleTypeNode* simpleType);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_SIMPLETYPENODE_H
