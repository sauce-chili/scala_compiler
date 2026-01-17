#include "SuperConstructorCallNode.h"

SuperConstructorCallNode::SuperConstructorCallNode(ArgumentExprsNode *argumentExprs) {
    this->argumentExprs = argumentExprs;
}

string SuperConstructorCallNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, argumentExprs, "argumentExprs_");

    return dot;
}

string SuperConstructorCallNode::getDotLabel() const {
    return "Super constructor call";
}
