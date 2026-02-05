#ifndef SCALA_LEXER_INFIXEXPRNODE_H
#define SCALA_LEXER_INFIXEXPRNODE_H


#include "../Node.h"
#include "../id/IdNode.h"
#include "../exprs/PrefixExprNode.h"
#include "IExpr.h"

class InfixExprNode: public Node, public IExpr {
public:
    InfixExprNode *left, *right;
    IdNode* fullId;
    PrefixExprNode* prefixExpr;

    bool visited = false;
    bool visitedForInfixOpTransform = false;

    InfixExprNode();

    InfixExprNode* copy();

    static InfixExprNode* createInfixFromPrefix(PrefixExprNode* prefixExpr);
    static InfixExprNode* createFromInfixes(InfixExprNode *left, IdNode* fullId, InfixExprNode *right);

    void transformInfixOperationToMethodCall();
    void normalizeInfix();

    /**
     * @brief Определяет тип данного инфиксного выражения.
     * @see IExpr::inferType
     */
    DataType inferType(
        ClassMetaInfo* currentClass,
        MethodMetaInfo* currentMethod,
        Scope* currentScope,
        int parentsConsider = PARENTS_CONSIDER // Просто большое число, без смысла
    ) const override;

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_INFIXEXPRNODE_H
