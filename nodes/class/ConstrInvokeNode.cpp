#include "ConstrInvokeNode.h"

ConstrInvokeNode::ConstrInvokeNode() {
    simpleType = nullptr;
    arguments = nullptr;
}

ConstrInvokeNode *ConstrInvokeNode::createWithArgumentsNode(SimpleTypeNode *simpleType, ArgumentExprsNode *arguments) {
    ConstrInvokeNode* node = new ConstrInvokeNode();
    node->simpleType = simpleType;
    node->arguments = arguments;
    return node;
}

string ConstrInvokeNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, simpleType, "simple type");
    addDotChild(dot, arguments, "arguments expressions");

    return dot;
}

string ConstrInvokeNode::getDotLabel() const {
    return "Constructor invoke";
}
