#ifndef SCALA_LEXER_BLOCKSTATSNODE_H
#define SCALA_LEXER_BLOCKSTATSNODE_H


#include "../Node.h"
#include "BlockStatNode.h"
#include "semantic/scopes/Scope.h"

#include <list>

class BlockStatsNode: public Node {
public:
    Scope *blockScope; // тип должен быть blockType
    std::list<BlockStatNode *> *blockStats;

    BlockStatsNode();

    BlockStatsNode(BlockStatNode *blockStat);

    static BlockStatsNode *addBlockStatToList(BlockStatsNode *list, BlockStatNode *blockStat);
    static BlockStatsNode *addBlockStatFrontToList(BlockStatsNode *list, BlockStatNode *blockStat);

    BlockStatsNode* copy();

    string toDot() const override;

    string getDotLabel() const override;

    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_BLOCKSTATSNODE_H
