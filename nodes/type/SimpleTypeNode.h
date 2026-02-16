#ifndef SCALA_LEXER_SIMPLETYPENODE_H
#define SCALA_LEXER_SIMPLETYPENODE_H

#include "../Node.h"
#include "../id/IdNode.h"

class SimpleTypeNode: public Node {
public:
    IdNode* fullId;
    SimpleTypeNode* arrayElemType;

    SimpleTypeNode();

    static SimpleTypeNode* createIdTypeNode(IdNode* fullId);
    static SimpleTypeNode* createSimpleTypeNode(SimpleTypeNode* simpleType);

    SimpleTypeNode* copy();

    string toDot() const override;
    string getDotLabel() const override;

    string getTypeName() const;

    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_SIMPLETYPENODE_H
