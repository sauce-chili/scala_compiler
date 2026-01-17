#ifndef SCALA_LEXER_SIMPLETYPENODE_H
#define SCALA_LEXER_SIMPLETYPENODE_H

#include "../Node.h"
#include "../id/StableIdNode.h"
#include "CompoundTypeNode.h"

class CompoundTypeNode;

class SimpleTypeNode: public Node {
public:
    StableIdNode* stableId;
    CompoundTypeNode* compoundType;

    SimpleTypeNode();

    static SimpleTypeNode* createStableIdNode(StableIdNode* stableId);
    static SimpleTypeNode* createArrayWithCompoundTypeNode(CompoundTypeNode* compoundType);

    SimpleTypeNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_SIMPLETYPENODE_H
