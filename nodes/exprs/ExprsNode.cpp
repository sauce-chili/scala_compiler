#include "ExprsNode.h"

ExprsNode::ExprsNode() {
    exprs = new std::list<ExprNode*>();
}

ExprsNode::ExprsNode(ExprNode *first) {
    exprs = new std::list<ExprNode*>();
    if (first) {
        exprs->push_back(first);
    }
}

ExprsNode *ExprsNode::addExprToList(ExprsNode *exprs, ExprNode *expr) {
    if (exprs == nullptr) {
        return new ExprsNode(expr);
    }

    if (expr) {
        exprs->exprs->push_back(expr);
    }
    return exprs;
}

ExprsNode *ExprsNode::copy() {
    ExprsNode* copied = new ExprsNode();

    if (exprs) {
        copied->exprs = new std::list<ExprNode*>();

        for (ExprNode* e: *exprs) {
            if (e)
                copied->exprs->push_back(e->copy());
            else
                copied->exprs->push_back(nullptr);
        }
    }

    return copied;
}

string ExprsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!exprs->empty()) {
        int i = 0;
        for (const auto *it : *exprs) {
            addDotChild(dot, it, "expr_" + to_string(i));
            i++;
        }
    }

    return dot;
}

string ExprsNode::getDotLabel() const {
    return "Exprs";
}

list<Node *> ExprsNode::getChildren() const {
    list<Node *> children = {};
    for (Node *it : *exprs) {
        addChildIfNotNull(children, it);
    }
    return children;
}

