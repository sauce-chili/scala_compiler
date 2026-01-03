#ifndef SCALA_COMPILER_NODE_H
#define SCALA_COMPILER_NODE_H

#include <string>

class ExprNode;
class StmtNode;
using namespace std;

class Node {
protected:
    static unsigned int maxId;

    void addDotNode(string &dot) const;
    void addDotChild(string &dot, const Node* child, const string &arrowLabel) const;

public:
    unsigned int id;
    Node() {id = ++maxId;};

    virtual string toDot() const = 0;
    virtual string getDotLabel() const = 0;
};


#endif