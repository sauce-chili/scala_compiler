#ifndef SCALA_LEXER_EXPRNODE_H
#define SCALA_LEXER_EXPRNODE_H

#include <list>
#include "../Node.h"
#include "../nodes/stats/BlockStatsNode.h"
#include "IExpr.h"

class ExprNode;
class TryExprNode;
class EnumeratorsNode;
class InfixExprNode;
class AssignmentNode;

class ExprNode: public Node, public IExpr {
public:
    ExprType type;
    std::list<ExprNode*> *exprs;
    TryExprNode* tryExpr;
    EnumeratorsNode* enumerators;
    InfixExprNode* infixExpr;
    AssignmentNode* assignment;

    ExprNode();

    ExprNode* copy();

    static ExprNode* createIfElse(ExprNode* cond, ExprNode* trueB, ExprNode* falseB);
    static ExprNode* createIf(ExprNode* cond, ExprNode* expr);
    static ExprNode* createWhile(ExprNode* cond, ExprNode* expr);
    static ExprNode* createTry(TryExprNode* tryExpr);
    static ExprNode* createDoWhile(ExprNode* cond, ExprNode* expr);
    static ExprNode* createThrow(ExprNode* expr);
    static ExprNode* createReturn();
    static ExprNode* createReturnExpr(ExprNode* expr);
    static ExprNode* createFor(EnumeratorsNode* enumerators, ExprNode* expr);
    static ExprNode* createForYield(EnumeratorsNode* enumerators, ExprNode* expr);
    static ExprNode* createInfix(InfixExprNode* infixExpr);
    static ExprNode* createAssignment(AssignmentNode* assignment);
    static ExprNode* createFromBlockStats(BlockStatsNode* blockStats);

    /**
     * @brief Infers the type of this expression.
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


#endif //SCALA_LEXER_EXPRNODE_H
