#ifndef SCALA_LEXER_SIMPLEEXPRNODE_H
#define SCALA_LEXER_SIMPLEEXPRNODE_H

#include "../Node.h"

class IdNode;
class ArgumentExprsNode;
class SimpleTypeNode;
class ConstrInvokeNode;
class SimpleExpr1Node;
class BlockStatsNode;

class SimpleExprNode: public Node {
public:
    SimpleExprType type;
    IdNode* fullId;
    ArgumentExprsNode* arguments;
    SimpleTypeNode* simpleType;
    BlockStatsNode* blockStats;
    SimpleExpr1Node* simpleExpr1;

    SimpleExprNode();

    static SimpleExprNode* createNewObjectNode(IdNode* fullId, ArgumentExprsNode* arguments);
    static SimpleExprNode* createNewArrayNode(SimpleTypeNode* simpleType, ArgumentExprsNode* arguments);
    static SimpleExprNode* createBlockStatsNode(BlockStatsNode* blockStats);
    static SimpleExprNode* createSimpleExpr1Node(SimpleExpr1Node* simpleExpr1);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_SIMPLEEXPRNODE_H
