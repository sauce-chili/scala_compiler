#ifndef SCALA_LEXER_SIMPLEEXPRNODE_H
#define SCALA_LEXER_SIMPLEEXPRNODE_H

#include "../Node.h"
#include "../class/ConstrInvokeNode.h"
#include "../stats/BlockStatsNode.h"
#include "SimpleExpr1Node.h"

class SimpleExpr1Node;

class SimpleExprNode: public Node {
public:
    SimpleExprType type;
    ConstrInvokeNode* constr;
    BlockStatsNode* blockStats;
    SimpleExpr1Node* simpleExpr1;

    SimpleExprNode();
    SimpleExprNode* clone() const;

    static SimpleExprNode* createConstrInvokeNode(ConstrInvokeNode* constr);
    static SimpleExprNode* createBlockStatsNode(BlockStatsNode* blockStats);
    static SimpleExprNode* createSimpleExpr1Node(SimpleExpr1Node* simpleExpr1);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_SIMPLEEXPRNODE_H
