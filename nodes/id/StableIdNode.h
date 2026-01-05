#ifndef SCALA_LEXER_STABLEIDNODE_H
#define SCALA_LEXER_STABLEIDNODE_H

#include "../Node.h"
#include "SingleStableIdNode.h"
#include <list>

class StableIdNode: public Node {
public:
    list<SingleStableIdNode*> *stableIds;

    StableIdNode(SingleStableIdNode *first);
    static StableIdNode* addStableId(StableIdNode* list, SingleStableIdNode* stableId);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_STABLEIDNODE_H
