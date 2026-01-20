#include "ConstrInvokeNode.h"

ConstrInvokeNode::ConstrInvokeNode() {
    fullId = nullptr;
    arguments = nullptr;
}

ConstrInvokeNode *ConstrInvokeNode::createConstrInvokeNode(IdNode* fullId, ArgumentExprsNode *arguments) {
    ConstrInvokeNode* node = new ConstrInvokeNode();
    node->fullId = fullId;
    node->arguments = arguments;
    return node;
}

ConstrInvokeNode *ConstrInvokeNode::copy() {
    ConstrInvokeNode* node = new ConstrInvokeNode();

    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (arguments) {
        node->arguments = arguments->copy();
    }

    return node;
}

string ConstrInvokeNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "id");
    addDotChild(dot, arguments, "arguments expressions");

    return dot;
}

string ConstrInvokeNode::getDotLabel() const {
    return "Constructor invoke";
}

list<Node *> ConstrInvokeNode::getChildren() const {
    std::list<Node *> children = {};
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, arguments);
    return children;
}
