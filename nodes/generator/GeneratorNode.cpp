#include "GeneratorNode.h"

GeneratorNode::GeneratorNode() {
    fullId = nullptr;
    simpleType = nullptr;
    expr = nullptr;
}

GeneratorNode *GeneratorNode::createGenerator(IdNode *fullId, SimpleTypeNode* simpleType, ExprNode *expr) {
    GeneratorNode* node = new GeneratorNode();
    node->fullId = fullId;
    node->simpleType = simpleType;
    node->expr = expr;
    return node;
}

string GeneratorNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, simpleType, "simpleType_");
    addDotChild(dot, expr, "expr_");

    return dot;
}

string GeneratorNode::getDotLabel() const {
    return "Generator";
}
