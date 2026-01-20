#include "SimpleExpr1Node.h"
#include "ArgumentExprsNode.h"
#include "SimpleExprNode.h"
#include "../id/IdNode.h"
#include "../type/SimpleTypeNode.h"

SimpleExpr1Node::SimpleExpr1Node() {
    intValue = 0;
    charValue = '\0';
    doubleValue = 0.0;
    stringValue = "";
    boolValue = false;

    simpleExpr = nullptr;
    expr = nullptr;
    argumentExprs = nullptr;
    simpleExpr1 = nullptr;
    identifier = nullptr;
    simpleType = nullptr;
}

SimpleExpr1Node* SimpleExpr1Node::createIntNode(int value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _INTEGER_LITERAL;
    node->intValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createCharNode(char value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _CHAR_LITERAL;
    node->charValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createDoubleNode(double value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _DOUBLE_LITERAL;
    node->doubleValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createStringNode(string value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _STRING_LITERAL;
    node->stringValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createBoolNode(bool value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _BOOL_LITERAL;
    node->boolValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createNullNode() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _NULL_LITERAL;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createIdNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _IDENTIFIER;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createMethodCallNode(SimpleExpr1Node* left, ArgumentExprsNode* right) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _METHOD_CALL;
    node->simpleExpr1 = left;
    node->argumentExprs = right;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createSuperFieldAccessNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _SUPER_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createThisFieldAccessNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _THIS_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createSimpleExprFieldAccessNode(IdNode* identifier, SimpleExprNode* expr) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _EXPRESSION_FIELD_ACCESS;
    node->identifier = identifier;
    node->simpleExpr = expr;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createArgumentCallNode(ExprNode* expr) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _ARGUMENT_CALL;
    node->expr = expr;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createEmptyCallNode() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _EMPTY_CALL;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createPlainThisNode() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _PLAIN_THIS;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createArrayWithTypeBuilderNode(SimpleTypeNode *simpleType,
                                                                 ArgumentExprsNode *argumentExprs) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _ARRAY_WITH_TYPE_BUILDER;
    node->simpleType = simpleType;
    node->argumentExprs = argumentExprs;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createArrayBuilderNode(ArgumentExprsNode *argumentExprs) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _ARRAY_BUILDER;
    node->argumentExprs = argumentExprs;
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
    addDotChild(dot, simpleType, "simpleType");

    return dot;
}

string SimpleExpr1Node::getDotLabel() const {
    switch (type) {
        case _INTEGER_LITERAL:    return to_string(intValue);
        case _DOUBLE_LITERAL:     return to_string(doubleValue);
        case _BOOL_LITERAL:       return to_string(boolValue);
        case _CHAR_LITERAL :      return to_string(charValue);
        case _STRING_LITERAL:     return stringValue;
        case _NULL_LITERAL:       return "null";

        default: break;
    }

    return simpleExpr1ToString(type);
}
