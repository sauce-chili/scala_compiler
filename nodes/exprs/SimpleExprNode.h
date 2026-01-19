#ifndef SCALA_LEXER_SIMPLEEXPRNODE_H
#define SCALA_LEXER_SIMPLEEXPRNODE_H

#include "../Node.h"

class ConstrInvokeNode;
class SimpleExpr1Node;
class BlockStatsNode;

class SimpleExprNode: public Node {
public:
    SimpleExprType type;
    ConstrInvokeNode* constr;
    BlockStatsNode* blockStats;
    SimpleExpr1Node* simpleExpr1;

    SimpleExprNode();

    SimpleExprNode* copy();

    static SimpleExprNode* createConstrInvokeNode(ConstrInvokeNode* constr);
    static SimpleExprNode* createBlockStatsNode(BlockStatsNode* blockStats);
    static SimpleExprNode* createSimpleExpr1Node(SimpleExpr1Node* simpleExpr1);
    static SimpleExprNode* createArrayCreatingNode(SimpleExpr1Node* simpleExpr1);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_SIMPLEEXPRNODE_H
