#ifndef SCALA_LEXER_PRIMARYCONSTRUCTORNODE_H
#define SCALA_LEXER_PRIMARYCONSTRUCTORNODE_H

#include "../Node.h"
#include "../func/FuncParamsNode.h"
#include "../stats/BlockStatsNode.h"
#include "SuperConstructorCallNode.h"

class PrimaryConstructorNode: public Node {
public:
    FuncParamsNode *funcParams;
    SuperConstructorCallNode *superCall;
    BlockStatsNode *blockStats;

    PrimaryConstructorNode* copy();

    PrimaryConstructorNode();
    PrimaryConstructorNode(FuncParamsNode *funcParams, BlockStatsNode *blockStats, SuperConstructorCallNode *superCall);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_PRIMARYCONSTRUCTORNODE_H
