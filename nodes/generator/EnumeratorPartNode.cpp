#include "EnumeratorPartNode.h"

EnumeratorPartNode::EnumeratorPartNode() {
    generator = nullptr;
    fullId = nullptr;
    compoundType = nullptr;
    expr = nullptr;
}

EnumeratorPartNode *EnumeratorPartNode::createGeneratorEnumeratorPart(GeneratorNode *generator) {
    EnumeratorPartNode* node = new EnumeratorPartNode();
    node->generator = generator;
    return node;
}

EnumeratorPartNode *EnumeratorPartNode::createVarDefEnumeratorPart(IdNode *fullId, CompoundTypeNode *compoundType, ExprNode *expr) {
    EnumeratorPartNode* node = new EnumeratorPartNode();
    node->fullId = fullId;
    node->compoundType = compoundType;
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
    if (compoundType) {
        node->compoundType = compoundType->copy();
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
    addDotChild(dot, compoundType, "compoundType_");
    addDotChild(dot, expr, "expr_");

    return dot;
}

string EnumeratorPartNode::getDotLabel() const {
    return generator ? "Enumerator part with generator" : "Enumerator part with var def";
}

