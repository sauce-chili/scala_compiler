#include "ExprsNode.h"

ExprsNode::ExprsNode() {

}

ExprsNode::ExprsNode(ExprNode *first) {
    exprs = new std::list<ExprNode*>();
    exprs->push_back(first);
}

ExprsNode *ExprsNode::addExprToList(ExprsNode *exprs, ExprNode *expr) {
    if (exprs == nullptr) {
        return new ExprsNode(expr);
    }

    exprs->exprs->push_back(expr);
    return exprs;
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