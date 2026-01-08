#include "IdNode.h"

IdNode *IdNode::createId(string name) {
    IdNode* node = new IdNode();
    node->name = name;
    return node;
}

IdNode *IdNode::createOperator(string op) {
    IdNode* node = new IdNode();
    node->name = op;
    return node;
}

string IdNode::toDot() const {
    string dot;

    addDotNode(dot);

    return dot;
}

string IdNode::getDotLabel() const {
    return "id: " + name;
}
