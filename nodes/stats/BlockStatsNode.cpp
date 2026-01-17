#include "BlockStatsNode.h"

BlockStatsNode::BlockStatsNode() {
    blockStats = new std::list<BlockStatNode *>;
}

BlockStatsNode::BlockStatsNode(BlockStatNode *blockStat) {
    blockStats = new std::list<BlockStatNode *>;
    if (blockStat) {
        blockStats->push_back(blockStat);
    }
}

BlockStatsNode *BlockStatsNode::addBlockStatToList(BlockStatsNode *list, BlockStatNode *blockStat) {
    if (list == nullptr) {
        return new BlockStatsNode(blockStat);
    }

    if (blockStat) {
        list->blockStats->push_back(blockStat);
    }
    return list;
}

string BlockStatsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!blockStats->empty()) {
        for (const auto *it: *blockStats) {
            addDotChild(dot, it, "blockStat_" + to_string(it->id));
        }
    }

    return dot;
}

string BlockStatsNode::getDotLabel() const {
    return "Block stats";
}

list<Node *> BlockStatsNode::getChildren() const {
    list<Node *> children = {};
    for (Node *node: *blockStats) {
        addChildIfNotNull(children, node);
    }
    return children;
}
