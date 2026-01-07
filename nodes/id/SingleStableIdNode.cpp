#include "SingleStableIdNode.h"

SingleStableIdNode *SingleStableIdNode::createStableIdByFullId(IdNode *identifier) {
    SingleStableIdNode* node = new SingleStableIdNode();
    node->type = _STABLE_ID_IDENTIFIER;
    node->identifier = identifier;
    return node;
}

SingleStableIdNode *SingleStableIdNode::createSuperCallStableId(IdNode *identifier) {
    SingleStableIdNode* node = new SingleStableIdNode();
    node->type = _STABLE_ID_SUPER_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

SingleStableIdNode *SingleStableIdNode::createThisCallStableIdBy(IdNode *identifier) {
    SingleStableIdNode* node = new SingleStableIdNode();
    node->type = _STABLE_ID_THIS_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

string SingleStableIdNode::toDot() const {
    string dot;

    addDotNode(dot);

    return dot;
}

string SingleStableIdNode::getDotLabel() const {
    string dot;

    dot += "stable id type: " + stableIdTypeToString(type) + ", identifier: " + identifier->name;

    return dot;
}
