#include "SuperConstructorCallNode.h"


SuperConstructorCallNode::SuperConstructorCallNode() {

}

SuperConstructorCallNode::SuperConstructorCallNode(ArgumentExprsNode *argumentExprs) {
    this->argumentExprs = argumentExprs;
}

SuperConstructorCallNode *SuperConstructorCallNode::copy() {
    SuperConstructorCallNode* node = new SuperConstructorCallNode();

    if (argumentExprs) {
        node->argumentExprs = argumentExprs->copy();
    }

    return node;
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