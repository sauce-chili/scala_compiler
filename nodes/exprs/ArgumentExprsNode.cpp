#include "ArgumentExprsNode.h"
#include "semantic/tables/tables.hpp"
#include "semantic/scopes/Scope.h"

ArgumentExprsNode::ArgumentExprsNode() {
    this->exprs = new ExprsNode();
}

ArgumentExprsNode::ArgumentExprsNode(ExprsNode* exprs) {
    this->exprs = exprs;
}

std::vector<DataType*> ArgumentExprsNode::getArgsTypes(
    ClassMetaInfo* currentClass,
    MethodMetaInfo* currentMethod,
    Scope* currentScope
) const {
    std::vector<DataType*> result;

    if (exprs && exprs->exprs) {
        for (ExprNode* expr : *exprs->exprs) {
            DataType exprType = expr->inferType(currentClass, currentMethod, currentScope);
            result.push_back(new DataType(exprType));
        }
    }

    return result;
}

string ArgumentExprsNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, exprs, "argument exprs");

    return dot;
}

string ArgumentExprsNode::getDotLabel() const {
    return "Argument exprs";
}

list<Node *> ArgumentExprsNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children,exprs);
    return children;
}

ArgumentExprsNode *ArgumentExprsNode::copy() {

    ExprsNode* copied = new ExprsNode();

    if (exprs) {
        copied->exprs = new std::list<ExprNode*>();

        for (ExprNode* e: *exprs->exprs) {
            if (e)
                copied->exprs->push_back(e->copy());
            else
                copied->exprs->push_back(nullptr);
        }
    }

    return new ArgumentExprsNode(copied);
}
