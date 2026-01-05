#include "SimpleExpr1Node.h"

SimpleExpr1Node::SimpleExpr1Node() {

}

SimpleExpr1Node* SimpleExpr1Node::clone() const {
    SimpleExpr1Node* copy = new SimpleExpr1Node();

    copy->type = this->type;
    copy->intValue = this->intValue;
    copy->doubleValue = this->doubleValue;
    copy->boolValue = this->boolValue;
    copy->charValue = this->charValue;
    copy->stringValue = this->stringValue;

    copy->simpleExpr = this->simpleExpr ? this->simpleExpr->clone() : nullptr;
    copy->simpleExpr1 = this->simpleExpr1 ? this->simpleExpr1->clone() : nullptr;
    copy->expr = this->expr ? this->expr->clone() : nullptr;
    //copy->argumentExprs = this->argumentExprs ? this->argumentExprs->clone() : nullptr;
    //copy->identifier = this->identifier ? this->identifier->clone() : nullptr;

    return copy;
}

SimpleExpr1Node* SimpleExpr1Node::createIntNode(int value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = INTEGER_LITERAL;
    node->intValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createCharNode(char value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = CHAR_LITERAL;
    node->charValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createDoubleNode(double value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = DOUBLE_LITERAL;
    node->doubleValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createStringNode(string value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = STRING_LITERAL;
    node->stringValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createBoolNode(bool value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = BOOL_LITERAL;
    node->boolValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createNullNode() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = NULL_LITERAL;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createIdNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = IDENTIFIER;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createMethodCallNode(SimpleExpr1Node* left, ArgumentExprsNode* right) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = METHOD_CALL;
    node->simpleExpr1 = left;
    node->argumentExprs = right;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createSuperFieldAccessNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = SUPER_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createThisFieldAccessNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = THIS_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createSimpleExprFieldAccessNode(IdNode* identifier, SimpleExprNode* expr) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = EXPRESSION_FIELD_ACCESS;
    node->identifier = identifier;
    node->simpleExpr = expr;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createArgumentCallNode(ExprNode* expr) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = ARGUMENT_CALL;
    node->expr = expr;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createEmptyCallNode() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = EMPTY_CALL;
    return node;
}

string SimpleExpr1Node::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, simpleExpr, "simple expr");
    addDotChild(dot, expr, "expr");
    addDotChild(dot, argumentExprs, "argument exprs");
    addDotChild(dot, simpleExpr1, "simple expr 1");
    addDotChild(dot, identifier, "identifier");

    return dot;
}

string SimpleExpr1Node::getDotLabel() const {
    switch (type) {
        case INTEGER_LITERAL:    return to_string(intValue);
        case DOUBLE_LITERAL:     return to_string(doubleValue);
        case BOOL_LITERAL:       return to_string(boolValue);
        case CHAR_LITERAL :      return to_string(charValue);
        case STRING_LITERAL:     return stringValue;
        case NULL_LITERAL:       return "null";

        default: break;
    }

    return simpleExpr1ToString(type);
}

