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
    addDotChild(dot, simpleType, "simpleType_");
    addDotChild(dot, expr, "expr_");

    return dot;
}

string GeneratorNode::getDotLabel() const {
    return "Generator";
}

list<Node *> GeneratorNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, compoundType);
    addChildIfNotNull(children, expr);
    return children;
}
