#ifndef SCALA_LEXER_SIMPLEEXPRNODE_H
#define SCALA_LEXER_SIMPLEEXPRNODE_H

#include "../Node.h"
#include "IExpr.h"

class IdNode;
class ArgumentExprsNode;
class SimpleTypeNode;
class SimpleExpr1Node;
class BlockStatsNode;

class SimpleExprNode: public Node, public IExpr {
public:
    SimpleExprType type;
    IdNode* fullId;
    ArgumentExprsNode* arguments;
    SimpleTypeNode* simpleType;
    BlockStatsNode* blockStats;
    SimpleExpr1Node* simpleExpr1;

    SimpleExprNode();

    SimpleExprNode* copy();

    static SimpleExprNode* createNewObjectNode(IdNode* fullId, ArgumentExprsNode* arguments);
    static SimpleExprNode* createNewArrayNode(SimpleTypeNode* simpleType, ArgumentExprsNode* arguments);
    static SimpleExprNode* createBlockStatsNode(BlockStatsNode* blockStats);
    static SimpleExprNode* createSimpleExpr1Node(SimpleExpr1Node* simpleExpr1);

    /**
     * @brief Определяет тип данного простого выражения.
     * @see IExpr::inferType
     */
    DataType inferType(
        ClassMetaInfo* currentClass,
        MethodMetaInfo* currentMethod,
        Scope* currentScope
    ) const override;

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_SIMPLEEXPRNODE_H
