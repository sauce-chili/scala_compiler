#include "Node.h"

unsigned int Node::seqId = 0;

void Node::addDotNode(string &dot) const {
    dot += "\t" + to_string(id) + " [label=\"" + getDotLabel() + "\"];\n";
}

void Node::addDotChild(string &dot, const Node *child, const string &arrowLabel) const {
    if (!child) {
        return;
    }

    dot += "\t" + to_string(id) + " -> " + to_string(child->id);

    if (!arrowLabel.empty()) {
        dot += " [label=\"" + arrowLabel + "\"]";
    }
    dot += ";\n" + child->toDot();
}
