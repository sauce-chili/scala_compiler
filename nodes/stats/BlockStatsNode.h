#ifndef SCALA_LEXER_BLOCKSTATSNODE_H
#define SCALA_LEXER_BLOCKSTATSNODE_H


#include "../Node.h"
#include "BlockStatNode.h"
#include "semantic/scopes/Scope.h"

class BlockStatsNode : public Node {
public:
    Scope *blockScope; // тип должен быть blockType
    std::list<BlockStatNode *> *blockStats;

    BlockStatsNode();

    BlockStatsNode(BlockStatNode *blockStat);

    static BlockStatsNode *addBlockStatToList(BlockStatsNode *list, BlockStatNode *blockStat);

    string toDot() const override;

    string getDotLabel() const override;
};


#endif //SCALA_LEXER_BLOCKSTATSNODE_H
