#ifndef SCALA_COMPILER_NODE_H
#define SCALA_COMPILER_NODE_H

#include <list>
#include <string>

#include "Types.h"

using namespace std;

class Node {
private:
    static unsigned int seqId;

protected:
    void addDotNode(string &dot) const;

    void addDotChild(string &dot, const Node *child, const string &arrowLabel) const;

    void addChildIfNotNull(std::list<Node *> &list, Node *node) const {
        if (node) list.push_back(node);
    }

public:
    const unsigned int id;

    Node() : id(++seqId) {
    }

    virtual string toDot() const = 0;

    virtual string getDotLabel() const = 0;

    virtual list<Node *> getChildren() const {
        return {};
    }
};


#endif
