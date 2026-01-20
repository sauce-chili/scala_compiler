#include "EnumeratorPartNode.h"

EnumeratorPartNode::EnumeratorPartNode() {
    generator = nullptr;
    fullId = nullptr;
    simpleType = nullptr;
    expr = nullptr;
}

EnumeratorPartNode *EnumeratorPartNode::createGeneratorEnumeratorPart(GeneratorNode *generator) {
    EnumeratorPartNode* node = new EnumeratorPartNode();
    node->generator = generator;
    return node;
}

EnumeratorPartNode *EnumeratorPartNode::createVarDefEnumeratorPart(IdNode *fullId, SimpleTypeNode* simpleType, ExprNode *expr) {
    EnumeratorPartNode* node = new EnumeratorPartNode();
    node->fullId = fullId;
    node->simpleType = simpleType;
    node->expr = expr;
    return node;
}

EnumeratorPartNode *EnumeratorPartNode::copy() {
    EnumeratorPartNode* node = new EnumeratorPartNode();
    if (generator) {
        node->generator = generator->copy();
    }
    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (simpleType) {
        node->simpleType = simpleType->copy();
    }
    if (expr) {
        node->expr = expr->copy();
    }
    return node;
}

string EnumeratorPartNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, generator, "generator_");
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, simpleType, "simpleType_");
    addDotChild(dot, expr, "expr_");

    return dot;
}

string EnumeratorPartNode::getDotLabel() const {
    return generator ? "Enumerator part with generator" : "Enumerator part with var def";
}

list<Node *> EnumeratorPartNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, generator);
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, simpleType);
    addChildIfNotNull(children, expr);
    return children;
}

