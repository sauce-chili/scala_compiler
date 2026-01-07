#ifndef SCALA_COMPILER_NODE_H
#define SCALA_COMPILER_NODE_H

#include <string>

#include "Types.h"

using namespace std;
class Node {
private:
    static unsigned int seqId;

protected:
    void addDotNode(string &dot) const;

    void addDotChild(string &dot, const Node *child, const string &arrowLabel) const;

public:
    const unsigned int id;

    Node() : id(++seqId) {
    }

    virtual string toDot() const = 0;

    virtual string getDotLabel() const = 0;
};


#endif
