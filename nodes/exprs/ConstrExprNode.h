#ifndef SCALA_LEXER_CONSTREXPRNODE_H
#define SCALA_LEXER_CONSTREXPRNODE_H


#include "../Node.h"
#include "../exprs/ArgumentExprsNode.h"
#include "../stats/BlockStatsNode.h"

class ConstrExprNode: public Node {
public:
    ArgumentExprsNode* argumentExprs;
    BlockStatsNode* blockStats;
    bool isBlock;

    ConstrExprNode();

    static ConstrExprNode* createConstrExpr(ArgumentExprsNode* argumentExprs, BlockStatsNode* blockStats, bool isBlock = false);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CONSTREXPRNODE_H
