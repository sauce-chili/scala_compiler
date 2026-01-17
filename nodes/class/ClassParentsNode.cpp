#include "ClassParentsNode.h"

ClassParentsNode::ClassParentsNode() {
    constr = nullptr;
    simpleTypes = nullptr;
}

ClassParentsNode *ClassParentsNode::createClassParents(ConstrInvokeNode *constr, SimpleTypesNode *simpleTypes) {
    ClassParentsNode* node = new ClassParentsNode();
    node->constr = constr;
    node->simpleTypes = simpleTypes;
    return node;
}

string ClassParentsNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, constr, "Constructor invoke");
    addDotChild(dot, simpleTypes, "Simple types");

    return dot;
}

string ClassParentsNode::getDotLabel() const {
    return "Class parents";
}

list<Node *> ClassParentsNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, constr);
    addChildIfNotNull(children, simpleTypes);
    return children;
}

