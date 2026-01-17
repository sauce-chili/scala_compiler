#include "ConstrInvokeNode.h"

ConstrInvokeNode::ConstrInvokeNode() {
    stableId = nullptr;
    arguments = nullptr;
}

ConstrInvokeNode *ConstrInvokeNode::createConstrInvokeNode(StableIdNode *stableId, ArgumentExprsNode *arguments) {
    ConstrInvokeNode *node = new ConstrInvokeNode();
    node->stableId = stableId;
    node->arguments = arguments;
    return node;
}

string ConstrInvokeNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, stableId, "stable id");
    addDotChild(dot, arguments, "arguments expressions");

    return dot;
}

string ConstrInvokeNode::getDotLabel() const {
    return "Constructor invoke";
}

list<Node *> ConstrInvokeNode::getChildren() const {
    std::list<Node *> children = {};
    addChildIfNotNull(children, stableId);
    addChildIfNotNull(children, arguments);
    return children;
}
