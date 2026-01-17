#include "ArgumentExprsNode.h"

ArgumentExprsNode::ArgumentExprsNode(ExprsNode* exprs) {
    this->exprs = exprs;
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
