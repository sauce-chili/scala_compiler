#ifndef SCALA_LEXER_COMPOUNDTYPENODE_H
#define SCALA_LEXER_COMPOUNDTYPENODE_H

#include <list>
#include "../Node.h"
#include "SimpleTypeNode.h"

class SimpleTypeNode;

class CompoundTypeNode: public Node {
public:
    list<SimpleTypeNode*> *simpleTypes;

    CompoundTypeNode(SimpleTypeNode *first);
    CompoundTypeNode();
    CompoundTypeNode(IdNode *first);

    static CompoundTypeNode* addStableId(CompoundTypeNode* list, SimpleTypeNode* simpleType);

    CompoundTypeNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_COMPOUNDTYPENODE_H
