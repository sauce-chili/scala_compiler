#include "GeneratorNode.h"

GeneratorNode::GeneratorNode() {
    fullId = nullptr;
    compoundType = nullptr;
    expr = nullptr;
}

GeneratorNode *GeneratorNode::createGenerator(IdNode *fullId, CompoundTypeNode *compoundType, ExprNode *expr) {
    GeneratorNode* node = new GeneratorNode();
    node->fullId = fullId;
    node->compoundType = compoundType;
    node->expr = expr;
    return node;
}

GeneratorNode *GeneratorNode::copy() {
    GeneratorNode* node = new GeneratorNode();
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

string GeneratorNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "fullId_");
    addDotChild(dot, compoundType, "compoundType_");
    addDotChild(dot, expr, "expr_");

    return dot;
}

string GeneratorNode::getDotLabel() const {
    return "Generator";
}
