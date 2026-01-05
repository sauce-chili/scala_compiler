#include "ArgumentExprsNode.h"

ArgumentExprsNode *ArgumentExprsNode::clone() const {
    return nullptr;
}

ArgumentExprsNode::ArgumentExprsNode(ExprsNode* exprs) {
    this->exprs = exprs;
}

string ArgumentExprsNode::toDot() const {
    string dot;

    addDotNode(dot);

    addDotChild(dot, exprs, "argument exprs");
}

string ArgumentExprsNode::getDotLabel() const {
    return "Argument exprs";
}
