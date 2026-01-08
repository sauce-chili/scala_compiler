#include "ExprNode.h"
#include "../try/TryExprNode.h"
#include "../generator/EnumeratorsNode.h"
#include "../exprs/InfixExprNode.h"
#include "../exprs/AssignmentNode.h"

ExprNode::ExprNode() {
    exprs = new std::list<ExprNode*>;
    tryExpr = nullptr;
    enumerators = nullptr;
    infixExpr = nullptr;
    assignment = nullptr;
}

ExprNode *ExprNode::createIfElse(ExprNode *cond, ExprNode *trueB, ExprNode *falseB) {
    ExprNode* node = new ExprNode();
    node->type = _IF_ELSE;
    node->exprs->push_back(cond);
    node->exprs->push_back(trueB);
    node->exprs->push_back(falseB);
    return node;
}

ExprNode *ExprNode::createIf(ExprNode *cond, ExprNode *expr) {
    ExprNode* node = new ExprNode();
    node->type = _IF;
    node->exprs->push_back(cond);
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createWhile(ExprNode *cond, ExprNode *expr) {
    ExprNode* node = new ExprNode();
    node->type = _WHILE;
    node->exprs->push_back(cond);
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createTry(TryExprNode *tryExpr) {
    ExprNode* node = new ExprNode();
    node->type = _TRY;
    node->tryExpr = tryExpr;
    return node;
}

ExprNode *ExprNode::createDoWhile(ExprNode *cond, ExprNode *expr) {
    ExprNode* node = new ExprNode();
    node->type = _DO_WHILE;
    node->exprs->push_back(cond);
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createThrow(ExprNode *expr) {
    ExprNode* node = new ExprNode();
    node->type = _THROW;
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createReturn() {
    ExprNode* node = new ExprNode();
    node->type = _RETURN_EMPTY;
    return node;
}

ExprNode *ExprNode::createReturnExpr(ExprNode *expr) {
    ExprNode* node = new ExprNode();
    node->type = _RETURN_EXPR;
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createFor(EnumeratorsNode *enumerators, ExprNode *expr) {
    ExprNode* node = new ExprNode();
    node->type = _FOR_WITHOUT_YIELD;
    node->exprs->push_back(expr);
    node->enumerators = enumerators;
    return node;
}

ExprNode *ExprNode::createForYield(EnumeratorsNode *enumerators, ExprNode *expr) {
    ExprNode* node = new ExprNode();
    node->type = _FOR_WITH_YIELD;
    node->exprs->push_back(expr);
    node->enumerators = enumerators;
    return node;
}

ExprNode *ExprNode::createInfix(InfixExprNode *infixExpr) {
    ExprNode* node = new ExprNode();
    node->type = _INFIX;
    node->infixExpr = infixExpr;
    return node;
}

ExprNode *ExprNode::createAssignment(AssignmentNode *assignment) {
    ExprNode* node = new ExprNode();
    node->type = _ASSIGNMENT;
    node->assignment = assignment;
    return node;
}

string ExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, tryExpr, "tryExpr_");
    addDotChild(dot, enumerators, "enumerators_");
    addDotChild(dot, infixExpr, "infixExpr_");
    addDotChild(dot, assignment, "assignment_");
    if (!exprs->empty()) {
        for (const auto *it : *exprs) {
            addDotChild(dot, it, "expr_" + to_string(it->id));
        }
    }

    return dot;
}

string ExprNode::getDotLabel() const {
    return exprTypeToString(type);
}
