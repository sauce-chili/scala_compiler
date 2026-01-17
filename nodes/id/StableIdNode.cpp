#include "StableIdNode.h"

StableIdNode::StableIdNode() {
    identifier = nullptr;
    stableId = nullptr;
}

StableIdNode *StableIdNode::createStableIdByFullId(IdNode *identifier) {
    StableIdNode* node = new StableIdNode();
    node->type = _STABLE_ID_IDENTIFIER;
    node->identifier = identifier;
    return node;
}

StableIdNode *StableIdNode::createSuperCallStableId(IdNode *identifier) {
    StableIdNode* node = new StableIdNode();
    node->type = _STABLE_ID_SUPER_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

StableIdNode *StableIdNode::createThisCallStableIdBy(IdNode *identifier) {
    StableIdNode* node = new StableIdNode();
    node->type = _STABLE_ID_THIS_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

StableIdNode *StableIdNode::createRecursiveStableId(StableIdNode *stableId, IdNode *identifier) {
    StableIdNode* node = new StableIdNode();
    node->stableId = stableId;
    node->type = _STABLE_ID_THIS_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

StableIdNode *StableIdNode::copy() {
    StableIdNode* node = new StableIdNode();

    if (stableId) {
        node->stableId = stableId->copy();
    }
    node->type = type;
    if (identifier) {
        node->identifier = identifier->copy();
    }

    return node;
}

string StableIdNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, identifier, "identifier_");
    addDotChild(dot, stableId, "stableId_");

    return dot;
}

string StableIdNode::getDotLabel() const {
    string dot;

    dot += "stable id type: " + stableIdTypeToString(type) + ", identifier: " + identifier->name;

    return dot;
}