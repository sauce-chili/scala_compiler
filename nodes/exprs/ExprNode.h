#ifndef SCALA_LEXER_EXPRNODE_H
#define SCALA_LEXER_EXPRNODE_H

#include <list>
#include "../Node.h"
#include "../try/TryExprNode.h"
#include "../generator/EnumeratorsNode.h"
#include "../exprs/InfixExprNode.h"
#include "../exprs/AssignmentNode.h"

class ExprNode: public Node {
public:
    ExprType type;
    std::list<ExprNode*> *exprs;
    TryExprNode* tryExpr;
    EnumeratorsNode* enumerators;
    InfixExprNode* infixExpr;
    AssignmentNode* assignment;

    ExprNode();

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

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_EXPRNODE_H
