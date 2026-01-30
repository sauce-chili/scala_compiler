#include "InfixExprNode.h"
#include "semantic/SemanticContext.h"
#include "semantic/error/SemanticError.h"
#include "semantic/tables/tables.hpp"

InfixExprNode::InfixExprNode() {
    left = nullptr;
    right = nullptr;
    fullId = nullptr;
    prefixExpr = nullptr;
}

InfixExprNode *InfixExprNode::createInfixFromPrefix(PrefixExprNode *prefixExpr) {
    InfixExprNode *node = new InfixExprNode();
    node->prefixExpr = prefixExpr;
    return node;
}

InfixExprNode *InfixExprNode::createFromInfixes(InfixExprNode *left, IdNode *fullId, InfixExprNode *right) {
    InfixExprNode *node = new InfixExprNode();
    node->left = left;
    node->fullId = fullId;
    node->right = right;
    return node;
}

InfixExprNode *InfixExprNode::copy() {
    InfixExprNode *node = new InfixExprNode();

    if (left) {
        node->left = left->copy();
    }
    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (right) {
        node->right = right->copy();
    }
    if (prefixExpr) {
        node->prefixExpr = prefixExpr->copy();
    }

    return node;
}

string InfixExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, prefixExpr, "prefixExpr_");
    addDotChild(dot, left, "left_");
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, right, "right_");

    return dot;
}

string InfixExprNode::getDotLabel() const {
    return prefixExpr ? "Infix expr from prefix" : "Infix expr with two infixes";
}

list<Node *> InfixExprNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, prefixExpr);
    addChildIfNotNull(children, left);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, right);
    return children;
}

DataType InfixExprNode::inferType(
    ClassMetaInfo *currentClass,
    MethodMetaInfo *currentMethod,
    Scope *currentScope
) const {
    // Имеется преобразование инфиксных вызов в классический вызов метода: a + b -> a.+(b)
    if (prefixExpr && !left && !right) {
        return prefixExpr->inferType(currentClass, currentMethod, currentScope);
    }

    // Бинарная операция (не должна остаться после трансформации, но на всякий случай)
    if (left && right && fullId) {
        DataType leftType = left->inferType(currentClass, currentMethod, currentScope);
        DataType rightType = right->inferType(currentClass, currentMethod, currentScope);

        std::string methodName = fullId->name;

        auto clsName = leftType.getClassName();

        auto classIt = ctx().classes.find(clsName);
        if (classIt != ctx().classes.end()) {
            std::vector<DataType *> argTypes;
            argTypes.push_back(new DataType(rightType));

            auto methodOpt = classIt->second->resolveMethod(methodName, argTypes, currentClass);
            for (auto *t: argTypes) delete t;

            if (methodOpt.has_value()) {
                return methodOpt.value()->returnType;
            }
        }

        throw SemanticError::MethodCandidateNotFound(id, methodName, "(" + rightType.toString() + ")");
    }

    throw SemanticError::InternalError(id, "Invalid InfixExprNode state");
}
