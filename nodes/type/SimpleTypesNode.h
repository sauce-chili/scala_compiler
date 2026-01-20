#ifndef SCALA_LEXER_SIMPLETYPESNODE_H
#define SCALA_LEXER_SIMPLETYPESNODE_H


#include "../Node.h"
#include "SimpleTypeNode.h"

#include <list>

class SimpleTypesNode: public Node {
public:
    std::list<SimpleTypeNode*> *simpleTypes;

    SimpleTypesNode();
    SimpleTypesNode(SimpleTypeNode* simpleType);

    static SimpleTypesNode* addSimpleTypeToList(SimpleTypesNode *list, SimpleTypeNode *simpleType);

    SimpleTypesNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_SIMPLETYPESNODE_H
