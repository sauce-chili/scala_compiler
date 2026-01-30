#include "ExprNode.h"
#include "../generator/EnumeratorsNode.h"
#include "../exprs/InfixExprNode.h"
#include "../exprs/AssignmentNode.h"
#include "semantic/SemanticContext.h"
#include "semantic/tables/tables.hpp"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"
#include "semantic/tools/datatype.h"

ExprNode::ExprNode() {
    exprs = new std::list<ExprNode *>;
    enumerators = nullptr;
    infixExpr = nullptr;
    assignment = nullptr;
}

ExprNode *ExprNode::createIfElse(ExprNode *cond, ExprNode *trueB, ExprNode *falseB) {
    ExprNode *node = new ExprNode();
    node->type = _IF_ELSE;
    node->exprs->push_back(cond);
    node->exprs->push_back(trueB);
    node->exprs->push_back(falseB);
    return node;
}

ExprNode *ExprNode::createIf(ExprNode *cond, ExprNode *expr) {
    ExprNode *node = new ExprNode();
    node->type = _IF;
    node->exprs->push_back(cond);
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createWhile(ExprNode *cond, ExprNode *expr) {
    ExprNode *node = new ExprNode();
    node->type = _WHILE;
    node->exprs->push_back(cond);
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createDoWhile(ExprNode *cond, ExprNode *expr) {
    ExprNode *node = new ExprNode();
    node->type = _DO_WHILE;
    node->exprs->push_back(cond);
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createThrow(ExprNode *expr) {
    ExprNode *node = new ExprNode();
    node->type = _THROW;
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createReturn() {
    ExprNode *node = new ExprNode();
    node->type = _RETURN_EMPTY;
    return node;
}

ExprNode *ExprNode::createReturnExpr(ExprNode *expr) {
    ExprNode *node = new ExprNode();
    node->type = _RETURN_EXPR;
    node->exprs->push_back(expr);
    return node;
}

ExprNode *ExprNode::createFor(EnumeratorsNode *enumerators, ExprNode *expr) {
    ExprNode *node = new ExprNode();
    node->type = _FOR_WITHOUT_YIELD;
    node->exprs->push_back(expr);
    node->enumerators = enumerators;
    return node;
}

ExprNode *ExprNode::createForYield(EnumeratorsNode *enumerators, ExprNode *expr) {
    ExprNode *node = new ExprNode();
    node->type = _FOR_WITH_YIELD;
    node->exprs->push_back(expr);
    node->enumerators = enumerators;
    return node;
}

ExprNode *ExprNode::createInfix(InfixExprNode *infixExpr) {
    ExprNode *node = new ExprNode();
    node->type = _INFIX;
    node->infixExpr = infixExpr;
    return node;
}

ExprNode *ExprNode::createAssignment(AssignmentNode *assignment) {
    ExprNode *node = new ExprNode();
    node->type = _ASSIGNMENT;
    node->assignment = assignment;
    return node;
}

string ExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, enumerators, "enumerators_");
    addDotChild(dot, infixExpr, "infixExpr_");
    addDotChild(dot, assignment, "assignment_");
    if (!exprs->empty()) {
        for (const auto *it: *exprs) {
            addDotChild(dot, it, "expr_" + to_string(it->id));
        }
    }

    return dot;
}

string ExprNode::getDotLabel() const {
    return exprTypeToString(type);
}

ExprNode *ExprNode::copy() {
    ExprNode* node = new ExprNode();

    if (exprs) {
        node->exprs = new std::list<ExprNode*>();

        for (ExprNode* e: *exprs) {
            if (e)
                node->exprs->push_back(e->copy());
            else
                node->exprs->push_back(nullptr);
        }
    }

    node->type = type;

    if (enumerators) {
        node->enumerators = enumerators->copy();
    }
    if (infixExpr) {
        node->infixExpr = infixExpr->copy();
    }
    if (assignment) {
        node->assignment = assignment->copy();
    }
    return node;
}

list<Node *> ExprNode::getChildren() const {
    list<Node *> children = {};

    switch (type) {
        case _FOR_WITHOUT_YIELD:
        case _FOR_WITH_YIELD:
            addChildIfNotNull(children, enumerators);
            if (exprs) {
                for (auto *e: *exprs) addChildIfNotNull(children, e);
            }
            break;
        case _IF_ELSE:
            // [0] - cond, [1] - then, [2] - else
            if (exprs) {
                for (auto *e: *exprs) addChildIfNotNull(children, e);
            }
            break;
        // [0] - cond, [1] - body
        case _IF:
        case _WHILE:
        case _DO_WHILE:
            if (exprs) {
                for (auto *e: *exprs) addChildIfNotNull(children, e);
            }
            break;
        case _INFIX:
            addChildIfNotNull(children, infixExpr);
            break;
        case _ASSIGNMENT:
            addChildIfNotNull(children, assignment);
            break;
        case _RETURN_EXPR:
        case _THROW: // уже не поддерживается
            if (exprs && !exprs->empty()) {
                addChildIfNotNull(children, exprs->front());
            }
            break;
        case _RETURN_EMPTY:
            // No children
            break;
        default:
            throw std::logic_error("Неизвестный ранее тип выражения");
            break;
    }

    return children;
}

DataType ExprNode::inferType(
    ClassMetaInfo* currentClass,
    MethodMetaInfo* currentMethod,
    Scope* currentScope
) const {
    switch (type) {
        case _INFIX:
            if (infixExpr) {
                return infixExpr->inferType(currentClass, currentMethod, currentScope);
            }
            throw SemanticError::InternalError(id, "ExprNode _INFIX without infixExpr");

        case _ASSIGNMENT:
            return DataType::makeUnit();

        case _IF_ELSE:
            // Type of if-else is the nearest common ancestor of both branches
            if (exprs && exprs->size() >= 3) {
                auto it = exprs->begin();
                ++it; // skip condition

                DataType thenType = (*it)->inferType(currentClass, currentMethod, currentScope);
                ++it;
                DataType elseType = (*it)->inferType(currentClass, currentMethod, currentScope);

                // Find nearest common ancestor
                auto commonTypeOpt = DataType::findCommonAncestor(thenType, elseType);

                if (commonTypeOpt.has_value()) {
                    return commonTypeOpt.value();
                }

                // Types are incompatible - throw error
                throw SemanticError::IfBranchesReturnDifferentTypes(
                    id,
                    thenType.toString(),
                    elseType.toString()
                );
            }
            throw SemanticError::InternalError(id, "ExprNode _IF_ELSE invalid state");

        case _IF:
        case _WHILE:
        case _DO_WHILE:
        case _FOR_WITHOUT_YIELD:
            return DataType::makeUnit();

        case _RETURN_EXPR:
            if (exprs && !exprs->empty()) {
                return exprs->front()->inferType(currentClass, currentMethod, currentScope);
            }
            return DataType::makeUnit();

        case _RETURN_EMPTY:
            return DataType::makeUnit();

        default:
            throw SemanticError::InternalError(id, "Unknown ExprNode type");
    }
}
