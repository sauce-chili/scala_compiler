#ifndef SCALA_LEXER_PREFIXEXPRNODE_H
#define SCALA_LEXER_PREFIXEXPRNODE_H


#include "../Node.h"
#include "SimpleExprNode.h"
#include "IExpr.h"

class PrefixExprNode: public Node, public IExpr {
public:
    PrefixExprType type;
    SimpleExprNode* simpleExpr;

    PrefixExprNode();

    PrefixExprNode* copy();

    static PrefixExprNode* createPrefixExprNode(SimpleExprNode* simpleExpr, PrefixExprType type);

    /**
     * @brief Определяет тип данного префиксного выражения.
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


#endif //SCALA_LEXER_PREFIXEXPRNODE_H
