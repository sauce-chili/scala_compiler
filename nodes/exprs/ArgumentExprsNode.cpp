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
