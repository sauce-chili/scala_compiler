#include "GeneratorNode.h"

GeneratorNode::GeneratorNode() {

}

GeneratorNode *GeneratorNode::createGenerator(IdNode *id, CompoundTypeNode *compoundType, ExprNode *expr) {
    GeneratorNode* node = new GeneratorNode();
    node->id = id;
    node->compoundType = compoundType;
    node->expr = expr;
    return node;
}

string GeneratorNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, id, "id_");
    addDotChild(dot, compoundType, "compoundType_");
    addDotChild(dot, expr, "expr_");

    return dot;
}

string GeneratorNode::getDotLabel() const {
    return "Generator";
}
