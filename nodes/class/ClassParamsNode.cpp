#include "ClassParamsNode.h"

ClassParamsNode::ClassParamsNode(ClassParamNode *first) {
    classParams = new std::list<ClassParamNode *>();
    if (first) {
        classParams->push_back(first);
    }
}

ClassParamsNode::ClassParamsNode() {
    classParams = new std::list<ClassParamNode *>();
}

ClassParamsNode *ClassParamsNode::addClassParamToList(ClassParamsNode *list, ClassParamNode *classParam) {
    if (list == nullptr) {
        return new ClassParamsNode(classParam);
    }

    if (classParam) {
        list->classParams->push_back(classParam);
    }
    return list;
}

string ClassParamsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!classParams->empty()) {
        int i = 0;
        for (const auto *it: *classParams) {
            addDotChild(dot, it, "classParam_" + to_string(i));
            i++;
        }
    }

    return dot;
}

string ClassParamsNode::getDotLabel() const {
    return "Class params";
}

list<Node *> ClassParamsNode::getChildren() const {
    std::list<Node *> children = {};
    for (Node *it: *classParams) {
        addChildIfNotNull(children, it);
    }
    return children;
}


list<Node *> ClassParamNode::getChildren() const {
    std::list<Node *> children = {};
    addChildIfNotNull(children, modifiers);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, compoundType);
    return children;
}
