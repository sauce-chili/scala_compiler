#include "PrefixExprNode.h"
#include "semantic/SemanticContext.h"
#include "semantic/tables/tables.hpp"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"

PrefixExprNode::PrefixExprNode() {
    simpleExpr = nullptr;
}

PrefixExprNode *PrefixExprNode::createPrefixExprNode(SimpleExprNode *simpleExpr, PrefixExprType type) {
    PrefixExprNode* node = new PrefixExprNode();
    node->type = type;
    node->simpleExpr = simpleExpr;
    return node;
}

PrefixExprNode *PrefixExprNode::copy() {
    PrefixExprNode* node = new PrefixExprNode();

    node->type = type;

    if (simpleExpr) {
        node->simpleExpr = simpleExpr->copy();
    }

    return node;
}

string PrefixExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, simpleExpr, "Simple expr");

    return dot;
}

string PrefixExprNode::getDotLabel() const {
    return prefixExprToString(type) + " prefix expr";
}

list<Node *> PrefixExprNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, simpleExpr);
    return children;
}

DataType PrefixExprNode::inferType(
    ClassMetaInfo* currentClass,
    MethodMetaInfo* currentMethod,
    Scope* currentScope
) const {
    if (!simpleExpr) {
        throw SemanticError::InternalError(id, "PrefixExprNode without simpleExpr");
    }

    // Get the type of the inner expression
    DataType innerType = simpleExpr->inferType(currentClass, currentMethod, currentScope);

    // No unary operator - just return the type
    if (type == _NO_UNARY_OPERATOR) {
        return innerType;
    }

    // Unary operators - resolve as methods
    std::string methodName;
    switch (type) {
        case _UNARY_PLUS:   methodName = "unary_+"; break;
        case _UNARY_MINUS:  methodName = "unary_-"; break;
        case _NOT:          methodName = "unary_!"; break;
        case _BIT_NOT:      methodName = "unary_~"; break;
        default:            return innerType;
    }

    // Resolve the unary operator method
    std::string className = innerType.getClassName();
    ClassMetaInfo* targetClass = nullptr;

    // Find class in context (works for both user-defined and RTL classes)
    auto classIt = ctx().classes.find(className);
    if (classIt != ctx().classes.end()) {
        targetClass = classIt->second;
    }

    if (targetClass) {
        std::vector<DataType*> emptyArgs;
        auto methodOpt = targetClass->resolveMethod(methodName, emptyArgs, currentClass);
        if (methodOpt.has_value()) {
            return methodOpt.value()->returnType;
        }
    }

    // Method not found - throw error
    throw SemanticError::MethodCandidateNotFound(simpleExpr->id, methodName, "()");
}
