#include "IdNode.h"

IdNode *IdNode::createId(string name) {
    IdNode* node = new IdNode();
    node->name = name;
    return node;
}

string IdNode::toDot() const {
    string dot;

    dot += "id: " + name;

    return dot;
}

string IdNode::getDotLabel() const {
    string dot;

    addDotNode(dot);

    return dot;
}

IdNode *IdNode::clone() const {
    IdNode* clone = new IdNode();
    clone->name = this->name;
    return clone;
}
