#include "EnumCaseNode.h"

EnumCaseNode::EnumCaseNode() {
    ids = nullptr;
    classParams = nullptr;
    classParents = nullptr;
}

EnumCaseNode* EnumCaseNode::createClassParents(EnumCaseType type, IdsNode *ids, ClassParamsNode *classParams, ClassParentsNode *classParents) {
    EnumCaseNode* node = new EnumCaseNode();
    node->type = type;
    node->ids = ids;
    node->classParams = classParams;
    node->classParents = classParents;
    return node;
}

string EnumCaseNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, ids, "Ids");
    addDotChild(dot, classParams, "Class params");
    addDotChild(dot, classParents, "Class parents");

    return dot;
}

string EnumCaseNode::getDotLabel() const {
    return classEnumCaseTypeToString(type);
}

list<Node *> EnumCaseNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, ids);
    addChildIfNotNull(children, classParams);
    addChildIfNotNull(children, classParents);
    return children;
}
