#include "EnumeratorsNode.h"
#include "semantic/scopes/Scope.h"

EnumeratorsNode::EnumeratorsNode() : generator(nullptr) {
}

EnumeratorsNode::EnumeratorsNode(GeneratorNode *generator) : generator(generator) {
}

string EnumeratorsNode::toDot() const {
    string dot;
    addDotNode(dot);
    return dot;
}

string EnumeratorsNode::getDotLabel() const {
    if (scope) {
        return "Enumerators\\nscopeId: " + to_string(scope->scopeId);
    }
    return "Enumerators";
}

list<Node *> EnumeratorsNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, generator);
    return children;
}

EnumeratorsNode *EnumeratorsNode::copy() {
    EnumeratorsNode* node = new EnumeratorsNode();
    if (generator) {
        node->generator = generator->copy();
    }
    return node;
}
