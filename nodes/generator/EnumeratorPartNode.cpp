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
