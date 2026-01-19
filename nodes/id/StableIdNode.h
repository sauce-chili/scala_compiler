#ifndef SCALA_LEXER_STABLEIDNODE_H
#define SCALA_LEXER_STABLEIDNODE_H

#include "../Node.h"
#include "IdNode.h"
#include "../Types.h"

class StableIdNode : public Node {
public:
    StableIdType type;
    IdNode *identifier;
    StableIdNode *stableId;

    StableIdNode();

    static StableIdNode *createStableIdByFullId(IdNode *identifier);

    static StableIdNode *createSuperCallStableId(IdNode *identifier);

    static StableIdNode *createThisCallStableIdBy(IdNode *identifier);

    static StableIdNode *createRecursiveStableId(StableIdNode *stableId, IdNode *identifier);

    StableIdNode* copy();

    string toDot() const override;

    string getDotLabel() const override;

    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_STABLEIDNODE_H
