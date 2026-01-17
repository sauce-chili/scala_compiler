#include "ArgumentExprsNode.h"

#include "nodes/enum/EnumStatsNode.h"

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

list<Node *> ArgumentExprsNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children,exprs);
    return children;
}
